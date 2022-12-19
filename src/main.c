#include "console.h"
#include "screen.h"

static inline void lcr3(unsigned int val){
    asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline void halt(void){
    asm volatile("hlt" : : );
}

void kernel_main(unsigned int* kernel_paging){
    screen_start();
    uartinit(); 
    printk("Hello World, DoomOS!\n");
    halt();
}


