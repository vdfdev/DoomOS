#include "console.h"
#include "screen.h"
#include "paging.h"

static inline void halt(void){
    asm volatile("hlt" : : );
}

void kernel_main(unsigned int* multiboot_info_ptr, unsigned int* page_tables_ptr){
    uartinit(); 
    paging_start(multiboot_info_ptr, page_tables_ptr);
    screen_start();
    printk("Hello World, DoomOS!\n");
    halt();
}
