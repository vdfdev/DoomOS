#include "console.h"
#include "screen.h"
#include "paging.h"

unsigned int* paging_ptr;
unsigned int* multiboot_info_ptr;

static inline void halt(void){
    asm volatile("hlt" : : );
}

void kernel_main(void){
    uartinit(); 
    paging_start();
    screen_start();
    printk("Hello World, DoomOS!\n");
    halt();
}
