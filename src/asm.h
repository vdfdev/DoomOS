#if !defined(__ASM_H__)
#define __ASM_H__

#include <stdint.h>

uint8_t asm_inb(uint16_t port);
void asm_outb(uint16_t port, uint8_t data);
void asm_halt();

#endif


