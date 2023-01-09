#include "uart.h"
#include "kernel.h"
#include <stdint.h>

#define COM1    0x3f8

static inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline __attribute__((always_inline)) uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static int uart;    // is there a uart?

void uart_init()
{

  // Turn off the FIFO
  outb(COM1+2, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  outb(COM1+3, 0x80);    // Unlock divisor
  outb(COM1+0, 115200/115200);
  outb(COM1+1, 0);
  outb(COM1+3, 0x03);    // Lock divisor, 8 data bits.
  outb(COM1+4, 0);
  outb(COM1+1, 0x01);    // Enable receive interrupts.

  // If status is 0xFF, no serial port.
  if(inb(COM1+5) == 0xFF)
      return;

  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  inb(COM1+2);
  inb(COM1+0);

  kprint("[UART] OK\r\n");
}

void
uart_putchar(char c)
{
  int i;

  if(!uart)
      return;
  
  for(i = 0; i < 128 && !(inb(COM1+5) & 0x20); i++) {
    continue; // Add microdelay?
  }

  outb(COM1+0, c);
}


