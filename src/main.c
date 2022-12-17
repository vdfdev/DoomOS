#include "console.h"
#include "screen.h"

static inline void lcr3(unsigned int val){
    asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline void halt(void){
    asm volatile("hlt" : : );
}

int main(void){
    screen_start();
    uartinit(); 
    printk("Hello World, QuakeOS!\n"); 
    halt();
}


