#if !defined(__PAGING_H__)
#define __PAGING_H__

void page_init(void* multiboot_info_ptr, void* page_tables_ptr);

#endif