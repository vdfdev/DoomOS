#include "uart.h"
#include "kernel.h"
#include "screen.h"
#include "page.h"
#include "gdt.h"
#include "pit.h"
#include "multiboot.h"
#include "interrupt.h"

static inline __attribute__((always_inline)) void halt()
{
    asm volatile ( "hlt" ::);
}

void kernel_main(void* multiboot_info_ptr){
  uart_init();
  gdt_init(); 
  page_init();
  multiboot_init(multiboot_info_ptr);
  interrupt_init();
  pit_init();
  screen_init();
  kprint("Welcome to DoomOS!\r\n");
  kprintf("Ticks: %u\r\n", ticks());
  for (;;) {
    kprintf("Sleep 5s, ticks: %u\r\n", ticks());
    msleep(5000);
  }
}
