#include "asm.h"

uint8_t asm_inb(uint16_t port)
{
    unsigned char data;

    asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    return data;
}

void asm_outb(uint16_t port, uint8_t data)
{
    asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

void asm_halt(){
    asm volatile("hlt" : : );
}
