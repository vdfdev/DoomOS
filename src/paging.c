#include "console.h"

// Reference: https://cdrdv2.intel.com/v1/dl/getContent/671200
#define PAGE_DIRECTORY_LENGTH 1024
#define PAGE_TABLE_LENGTH 1024
// 64 page tables maps 256 MB of memory:
// 64 page tables * 1024 (page / page table) * 4 KB / page = 256 MB 
#define VALID_PAGE_TABLE_COUNT 64

// Vol 3, Chapter 4 Paging, Hierarchical Paging Structures.
// The Page directory has 1024 page directory entries (PDE).
unsigned int page_directory[PAGE_DIRECTORY_LENGTH]; // 4 KB
// Each page table has 1024 page table entries (PTE).
// Each PTE maps 4kb of memory.
unsigned int page_table[PAGE_TABLE_LENGTH * VALID_PAGE_TABLE_COUNT];

void invalidate_all_pdes() {
  for (int i = 0; i < PAGE_DIRECTORY_LENGTH; i++) {
    page_directory[i] = 0;
  }
}

void invalidate_all_ptes() {
  for (int i = 0; i < PAGE_TABLE_LENGTH; i++) {
    page_table[i] = 0;
  }
}

void init_page_directory(int page_table_index) {
    // Bit 0 as 1 makes it valid.
    // Bit 1 as 1 makes it writable.
    unsigned int flags = 0b11;
    unsigned int address = page_table[PAGE_TABLE_LENGTH * page_table_index];
    page_directory[page_table_index] = address | flags;
}

void paging_start(unsigned int* paging, unsigned int* multiboot_info) {
   printk("paging_start\n");
  /*invalidate_all_pdes();
  invalidate_all_ptes();
  for (int i = 0; i < PAGE_DIRECTORY_LENGTH; i++) {
    init_page_directory(i);
    for (int j = 0; j < VALID_PAGE_TABLE_COUNT; j++) {
      // map_identity_page(i, j);
    }
  }*/
}