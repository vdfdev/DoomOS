// Based on https://github.com/pdoane/osdev/blob/master/time/pit.c
#include "pit.h"
#include <stdint.h>

static inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline __attribute__((always_inline)) void halt()
{
    asm volatile ( "hlt" ::);
}

static inline __attribute__((always_inline)) uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

volatile uint32_t pit_ticks;

// I/O Ports
#define PIT_COUNTER0                    0x40
#define PIT_CMD                         0x43

// BCD
#define CMD_BINARY                      0x00    // Use Binary counter values
#define CMD_BCD                         0x01    // Use Binary Coded Decimal counter values

// Mode
#define CMD_MODE0                       0x00    // Interrupt on Terminal Count
#define CMD_MODE1                       0x02    // Hardware Retriggerable One-Shot
#define CMD_MODE2                       0x04    // Rate Generator
#define CMD_MODE3                       0x06    // Square Wave
#define CMD_MODE4                       0x08    // Software Trigerred Strobe
#define CMD_MODE5                       0x0a    // Hardware Trigerred Strobe

// Read/Write
#define CMD_LATCH                       0x00
#define CMD_RW_LOW                      0x10    // Least Significant Byte
#define CMD_RW_HI                       0x20    // Most Significant Byte
#define CMD_RW_BOTH                     0x30    // Least followed by Most Significant Byte

// Counter Select
#define CMD_COUNTER0                    0x00
#define CMD_COUNTER1                    0x40
#define CMD_COUNTER2                    0x80
#define CMD_READBACK                    0xc0

#define PIT_FREQUENCY                   1193182

void pit_init()
{
  uint32_t hz = 1000;
  uint32_t divisor = PIT_FREQUENCY / hz;
  outb(PIT_CMD, CMD_BINARY | CMD_MODE3 | CMD_RW_BOTH | CMD_COUNTER0);
  outb(PIT_COUNTER0, divisor);
  outb(PIT_COUNTER0, divisor >> 8);
}

void pit_tick()
{
  pit_ticks++;
}

uint32_t pit_get_ticks()
{
  return pit_ticks;
}

void pit_wait(uint32_t ms)
{
  uint32_t start = pit_ticks;
 
  while (pit_ticks - start < ms)
  {
    halt();
  }
}