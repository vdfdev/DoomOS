#include "console.h"
#include "screen.h"
#include "paging.h"

unsigned int* multiboot_info_ptr;

static inline void halt(void){
    asm volatile("hlt" : : );
}

void kernel_main(unsigned int* page_tables_ptr){
    uartinit(); 
    paging_start();
    screen_start();
    printk("Hello World, DoomOS!\n");
    halt();
}
