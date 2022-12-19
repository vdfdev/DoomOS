#include "console.h"
#include "screen.h"

unsigned int* paging_ptr;

static inline void lcr3(unsigned int val){
    asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline void halt(void){
    asm volatile("hlt" : : );
}

void kernel_main(void){
    screen_start();
    uartinit(); 
    printk("Hello World, DoomOS!\n");
    halt();
}


