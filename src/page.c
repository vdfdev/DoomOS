#include "uart.h"
#include "kernel.h"
#include <stdint.h>
#include <stdbool.h>

// Reference: https://cdrdv2.intel.com/v1/dl/getContent/671200
#define PAGE_DIRECTORY_LENGTH 1024
#define PAGE_TABLE_LENGTH 1024
// 2 page tables maps 8 MB of memory:
// 2 page tables * 1024 (page / page table) * 4 KB / page = 8 MB 
#define KERNEL_PAGE_TABLE_COUNT 2

// CR3 is used to determine the 4k-address aligned (20 bits) base of the page directory.
struct page_cr3 {
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
struct page_pde {
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
}__attribute__((packed));

// Page Table Entry. Specifies a page (4KB) attributes.
struct page_pte {
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
}__attribute__((packed));

// Basic physical memory map:                                                                                                                                                             
// [0   , 1 MB)           We're just leaving this alone for now (VGA, etc).                                                                                                                        
// [1 MB, 8 MB)           Used/Reserved by Kernel.                                                
// [8 MB, MAX ]           Userspace physical pages (available for allocation!)                                                                                                          
                                                                                                                                                                                                
// Basic virtual memory map:                                                                                                                                                              
// 0 -> 4 KB                Null page (so nullptr dereferences crash!)                                                                                                                    
// 4 KB -> 8 MB             Identity mapped.                                                                                                                                              
// 8 MB -> 3 GB             Available to userspace.                                                                                                                                       
// 3GB  -> 4 GB             Kernel-only virtual address space (>0xc0000000)                                                                                                               

void page_identity_map(struct page_pte* pte) {
  pte->present = true;
  pte->writable = true;
  // TODO: set address to be identity mapped.
}

// Page tables pointer need to be reserved by linker so we can be sure it is 4kb-aligned.
void page_init(void* page_tables_ptr) {
  // Zero all PDEs and PTEs
  for (int i = 0; i < PAGE_DIRECTORY_LENGTH + PAGE_TABLE_LENGTH * KERNEL_PAGE_TABLE_COUNT; i++) {
    *((uint32_t*)page_tables_ptr + i) = 0;
  }
  // Setup kernel (8MB identity mapping).
  struct page_pde* page_directory = (struct page_pde*)page_tables_ptr;
  struct page_pte* page_tables_base = (struct page_pte*)(page_tables_ptr + PAGE_DIRECTORY_LENGTH);
  for (int i = 0; i < KERNEL_PAGE_TABLE_COUNT; i++) {
    struct page_pde* pde = page_directory + i;
    pde->present = true;
    pde->writable = true;
    struct page_pte* page_table = page_tables_base + i * PAGE_TABLE_LENGTH;
    pde->page_table_address = ((uint32_t)page_table) << 12;
    for (int j = 0; j < PAGE_TABLE_LENGTH; j++) {
      struct page_pte* pte = page_table + j;
      page_identity_map(pte);
    }
  }
  // Set first 4KB as invalid (null page).
  page_tables_base->present = false;
  // Make first page null page, so nullptr derefences crash.
  printk("[PAGE] OK\n");
}