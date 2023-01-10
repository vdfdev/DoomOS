#include "uart.h"
#include "kernel.h"
#include "screen.h"
#include "page.h"
#include "gdt.h"
#include "pit.h"
#include "multiboot.h"
#include "interrupt.h"
#include "terminal.h"

static inline __attribute__((always_inline)) void halt()
{
    asm volatile ( "hlt" ::);
}

void kernel_welcome() {
  // Logo credits to https://www.gamers.org/~fpv/doomlogo.html
  kprint("=================     ===============     ===============   ========  ========\r\n");
  kprint("\\\\ . . . . . . .\\\\   //. . . . . . .\\\\   //. . . . . . .\\\\  \\\\. . .\\\\// . . //\r\n");
  kprint("||. . ._____. . .|| ||. . ._____. . .|| ||. . ._____. . .|| || . . .\\/ . . .||\r\n");
  kprint("|| . .||   ||. . || || . .||   ||. . || || . .||   ||. . || ||. . . . . . . ||\r\n");
  kprint("||. . ||   || . .|| ||. . ||   || . .|| ||. . ||   || . .|| || . | . . . . .||\r\n");
  kprint("|| . .||   ||. _-|| ||-_ .||   ||. . || || . .||   ||. _-|| ||-_.|\\ . . . . ||\r\n");
  kprint("||. . ||   ||-'  || ||  `-||   || . .|| ||. . ||   ||-'  || ||  `|\\_ . .|. .||\r\n");
  kprint("|| . _||   ||    || ||    ||   ||_ . || || . _||   ||    || ||   |\\ `-_/| . ||\r\n");
  kprint("||_-' ||  .|/    || ||    \\|.  || `-_|| ||_-' ||  .|/    || ||   | \\  / |-_.||\r\n");
  kprint("||    ||_-'      || ||      `-_||    || ||    ||_-'      || ||   | \\  / |  `||\r\n");
  kprint("||    `'         || ||         `'    || ||    `'         || ||   | \\  / |   ||\r\n");
  kprint("||            .===' `===.         .==='.`===.         .===' /==. |  \\/  |   ||\r\n");
  kprint("||         .=='   \\_|-_ `===. .==='   _|_   `===. .===' _-|/   `==  \\/  |   ||\r\n");
  kprint("||      .=='    _-'    `-_  `='    _-'   `-_    `='  _-'   `-_  /|  \\/  |   ||\r\n");
  kprint("||   .=='    _-'          `-__\\._-'         `-_./__-'         `' |. /|  |   ||\r\n");
  kprint("||.=='    _-'                      ____  _____                    `' |  /==.||\r\n");
  kprint("=='    _-'                        / __ \\/ ___/                        \\/   `==\r\n");
  kprint("\\   _-'                          / / / /\\__ \\                          `-_   /\r\n");
  kprint("`''                             / /_/ /___/ /                              ``'\r\n");
  kprint("                                \\____//____/                                  \r\n");
  kprint("\r\n");
  kprint("  -> Welcome to DoomOS! Learning About Operating System By Running DOOM <-\r\n");
  kprint("\r\n");
}

void kernel_main(void* multiboot_info_ptr){
  uart_init();
  gdt_init(); 
  page_init();
  multiboot_init(multiboot_info_ptr);
  interrupt_init();
  pit_init();
  kernel_welcome();
  screen_refresh();
  for (;;) {
    halt();
  }
}
