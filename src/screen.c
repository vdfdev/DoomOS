#include <stdint.h>
#include "kernel.h"
#include "uart.h"

const uint8_t SCREEN_COLS = 80;
const uint8_t SCREEN_ROWS = 25;

void write_char(uint8_t x, uint8_t y, char c, char attrib) {
  *((char*)0xB8000 + (y * SCREEN_COLS + x) * 2) = c;
  *((char*)0xB8000 + (y * SCREEN_COLS + x) * 2 + 1) = attrib;
}

void clear_screen() {
  for (uint8_t i = 0; i < SCREEN_COLS; i++) {
    for (uint8_t j = 0; j < SCREEN_ROWS; j++) {
      write_char(i, j, ' ', 0);
    }
  }
}

void write_cute_str(uint8_t x, uint8_t y, char* s) {
  uint8_t j = 0;
  for (uint32_t i = 0; *(s+i) != 0; i++) {
    char c = *(s+i);
    write_char(x + i, y, c, 2 + (j%4));
    j++;
    if (c == ' ') {
      j = 0;
    }
  }
}

void write_str(uint8_t x, uint8_t y, char* s) {
  for (uint32_t i = 0; *(s+i) != 0; i++) {
    write_char(x + i, y, *(s+i), 7);
  }
}

void screen_init() {
  clear_screen();
  write_cute_str(30, 1, "Hello World, DoomOS!");
  write_str(30, 2, "A Didatic OS.");
  write_str(0, 4, "Learning about Operating Systems and computers by running Doom.");
  kprint("[SCREEN] OK\r\n");
}