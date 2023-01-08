#include "uart.h"
#include "kernel.h"
#include "asm.h"

#define COM1    0x3f8

static int uart;    // is there a uart?

void uart_init()
{

  // Turn off the FIFO
  asm_outb(COM1+2, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  asm_outb(COM1+3, 0x80);    // Unlock divisor
  asm_outb(COM1+0, 115200/115200);
  asm_outb(COM1+1, 0);
  asm_outb(COM1+3, 0x03);    // Lock divisor, 8 data bits.
  asm_outb(COM1+4, 0);
  asm_outb(COM1+1, 0x01);    // Enable receive interrupts.

  // If status is 0xFF, no serial port.
  if(asm_inb(COM1+5) == 0xFF)
      return;

  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  asm_inb(COM1+2);
  asm_inb(COM1+0);

  kprint("[UART] OK\r\n");
}

void
uart_putchar(char c)
{
  int i;

  if(!uart)
      return;
  
  for(i = 0; i < 128 && !(asm_inb(COM1+5) & 0x20); i++) {
    continue; // Add microdelay?
  }

  asm_outb(COM1+0, c);
}


