#if !defined(__PAGING_H__)
#define __PAGING_H__

void paging_start(unsigned int* multiboot_info_ptr, unsigned int* page_tables_ptr);

#endif