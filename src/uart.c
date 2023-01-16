#include "uart.h"
#include "kernel.h"
#include <stdint.h>
#include <stdbool.h>

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

static bool uart = false;    // is there a uart?

void uart_init()
{
  outb(COM1 + 1, 0x00);    // Disable all interrupts
  outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 115200/115200);    // Set divisor to 1 (lo byte) 115200 baud
  outb(COM1 + 1, 0x00);    //                           (hi byte)
  outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit, disable DLAB
  outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4, 0x0F);
  outb(COM1 + 1, 0x1); // Enable interrupts
  if (inb(COM1+5) == 0xFF) {
    return;
  }
  uart = true;
  kprint("[UART] OK\r\n");
}

void
uart_putchar(char c)
{
  int i;

  if(!uart)
      return;
  
  for(i = 0; i < 128 && !(inb(COM1+5) & 0x20); i++) {
    msleep(1);
    continue;
  }

  outb(COM1+0, c);
}
