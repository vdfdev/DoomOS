#include "uart.h"
#include "kernel.h"
#include <stdint.h>
#include <stdbool.h>

// Reference: https://cdrdv2.intel.com/v1/dl/getContent/671200
#define PAGE_DIRECTORY_LENGTH 1024
#define PAGE_TABLE_LENGTH 1024
#define PAGE_SIZE 4096
// 2 page tables maps 8 MB of memory:
// 2 page tables * 1024 (page / page table) * 4 KB / page = 8 MB 
#define KERNEL_PAGE_TABLE_COUNT 2

// CR3 is used to determine the 4k-address aligned (20 bits) base of the page directory.
struct page_cr3_t {
  // Ignored.
  uint32_t : 3;
  // Page-level write-through; indirectly determines the memory type used to access the page 
  // directory during linear-address translation.
  bool pwt : 1;
  // Page-level cache disable; indirectly determines the memory type used to access the page 
  // directory during linear-address translation
  bool pcd : 1;
  // Ignored.
  uint32_t : 7;
  // Physical address of the 4-KByte aligned page directory used for linear-address translation.
  // We only need 20 bits for the address because we know that the last 12 bits is zero (4K aligned).
  uint32_t page_directory_address : 20;
} __attribute__((packed));

// Page Directory Entry. Points to Page Table based on the last (most significant) 10 bits of the address.
// Each Page Table has 1024 4KB pages, therefore each PDE takes care of 4MB of memory. 
struct page_pde_t {
  // Present; must be 1 to map a 4-MByte page. If false, all the other bits are ignored.
  bool present : 1;
  // Read/write; if 0, writes may not be allowed to the 4-MByte page referenced by this entry.
  bool writable : 1;
  // User/supervisor; if 0, user-mode accesses are not allowed to the 4-MByte page referenced by this entry.
  bool user_accessible : 1;
  // Page-level write-through; indirectly determines the memory type used to access the 4-MByte page 
  // referenced by this entry (see Section 4.9).
  bool pwt : 1;
  // Page-level cache disable; indirectly determines the memory type used to access the 4-MByte page 
  // referenced  by this entry (see Section 4.9).
  bool pcd : 1;
  // Accessed; indicates whether software has accessed the 4-MByte page referenced by this entry.
  bool accessed : 1;
  // Ignored.
  uint32_t : 1;
  // If CR4.PSE = 1, must be 0 (otherwise, this entry maps a 4-MByte page; see Table 4-4);
  // otherwise, ignored. (Must be 0 in our case)
  bool ps : 1;
  // Ignored.
  uint32_t : 4;
  // Physical address of 4-KByte aligned page table referenced by this entry.
  // We only need 20 bits for the address because we know that the last 12 bits is zero (4K aligned).
  uint32_t page_table_address : 20;
} __attribute__((packed));

struct page_directory_t {
  struct page_pde_t entries[PAGE_DIRECTORY_LENGTH];
} __attribute__((packed));

// Page Table Entry. Specifies a page (4KB) attributes.
struct page_pte_t {
  // Present; must be 1 to map a 4KB page. If false, all the other bits are ignored.
  bool present : 1;
  // Read/write; if 0, writes may not be allowed to the 4KB page referenced by this entry.
  bool writable : 1;
  // User/supervisor; if 0, user-mode accesses are not allowed to the 4KB page referenced by this entry.
  bool user_accessible : 1;
  // Page-level write-through; indirectly determines the memory type used to access the 4KB page 
  // referenced by this entry (see Section 4.9).
  bool pwt : 1;
  // Page-level cache disable; indirectly determines the memory type used to access the 4KB page 
  // referenced  by this entry (see Section 4.9).
  bool pcd : 1;
  // Accessed; indicates whether software has accessed the 4KB page referenced by this entry.
  bool accessed : 1;
  // Dirty; indicates whether software has written to the 4-KByte page referenced by this entry.
  bool dirty : 1;
  // If the PAT is supported, indirectly determines the memory type used to access the 4-KByte page 
  // referenced by this entry (see Section 4.9.2); otherwise, reserved (must be 0).
  bool pat : 1;
  // Global; if CR4.PGE = 1, determines whether the translation is global (see Section 4.10);
  // ignored otherwise
  bool g : 1;
  // Ignored.
  uint32_t : 3;
  // Physical address of 4-KByte aligned page referenced by this entry.
  // We only need 20 bits for the address because we know that the last 12 bits is zero (4K aligned).
  uint32_t page_address : 20;
} __attribute__((packed));

struct page_table_t {
  struct page_pte_t entries[PAGE_TABLE_LENGTH];
} __attribute__((packed));

// Basic physical memory map:                                                                                                                                                             
// [0   , 1 MB)           We're just leaving this alone for now (VGA, etc).                                                                                                                        
// [1 MB, 8 MB)           Used/Reserved by Kernel.                                                
// [8 MB, MAX ]           Userspace physical pages (available for allocation!)                                                                                                          
                                                                                                                                                                                                
// Basic virtual memory map:                                                                                                                                                              
// 0 -> 4 KB                Null page (so nullptr dereferences crash!)                                                                                                                    
// 4 KB -> 8 MB             Identity mapped.                                                                                                                                              
// 8 MB -> 3 GB             Available to userspace.                                                                                                                                       
// 3GB  -> 4 GB             Kernel-only virtual address space (>0xc0000000)                                                                                                               

void page_identity_map(struct page_pte_t* pte, uint16_t pde_index, uint16_t pte_index) {
  pte->present = true;
  pte->writable = true;
  // Address is PAGE_SIZE * PAGE_TABLE_LENGTH * pde_index + PAGE_SIZE * pte_index
  // but we lose the 12 least significant bits (because it is always 4kb aligned)
  // which is equivalent of dividing by 4096, so we can simplify the address to
  pte->page_address = PAGE_TABLE_LENGTH * pde_index + pte_index;
}

__attribute__((aligned(4096)))
struct page_directory_t page_directory;
__attribute__((aligned(4096)))
struct page_table_t kernel_page_tables[KERNEL_PAGE_TABLE_COUNT];

void page_flush() {
  // Enable paging
  struct page_cr3_t cr3;
  cr3.pwt = false;
  cr3.pcd = false;
  cr3.page_directory_address = ((uint32_t)&page_directory) / PAGE_SIZE; 
  asm volatile(
    "mov %0, %%eax\n"
    "mov %%eax, %%cr3\n"
    "mov %%cr0, %%eax\n"
    "or $0x80000000, %%eax\n"
    "mov %%eax, %%cr0"
  :: "a"(cr3) : "memory");
}


void page_init_directory() {
  for (uint16_t i = 0; i < PAGE_DIRECTORY_LENGTH; i++) {
    struct page_pde_t* pde = &(page_directory.entries[i]);
    *((uint32_t*)pde) = 0;
  }
}

void page_init_single_page_table(struct page_table_t* pt) {
  for (uint16_t i = 0; i < PAGE_TABLE_LENGTH; i++) {
    struct page_table_entry_t* pte = &(pt->entries[i]);
    *((uint32_t*)pte) = 0;
  }
}

void page_init_kernel_page_tables() {
  for (uint16_t i = 0; i < KERNEL_PAGE_TABLE_COUNT; i++) {
    struct page_table_t* page_table = &(kernel_page_tables[i]);
    page_init_single_page_table(page_table);
  }
}

void page_identity_map_whole_page_table(struct page_table_t* page_table, uint16_t pde_index) {
  for (uint16_t i = 0; i < PAGE_TABLE_LENGTH; i++) {
    struct page_pte_t* pte = &(page_table->entries[i]);
    page_identity_map(pte, pde_index, i);
  }
}

// Page tables pointer need to be reserved by linker so we can be sure it is 4kb-aligned.
void page_init() {
  page_init_directory();
  page_init_kernel_page_tables();
  for (uint16_t i = 0; i < KERNEL_PAGE_TABLE_COUNT; i++) {
    struct page_pde_t* pde = &(page_directory.entries[i]);
    pde->present = true;
    pde->writable = true;
    struct page_table_t* page_table = &(kernel_page_tables[i]);
    pde->page_table_address = ((uint32_t)page_table) / PAGE_SIZE;
    page_identity_map_whole_page_table(page_table, i);
  }
  // Set first 4KB as invalid (null page).
  kernel_page_tables[0].entries[0].present = false;
  page_flush();
  kprint("[PAGE] OK\n");
}