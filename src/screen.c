#include <stdint.h>
#include <stdbool.h>
#include "kernel.h"
#include "terminal.h"

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

void write_line(uint8_t x, uint8_t y, char* s) {
  bool line_ended = false;
  for (uint32_t i = 0; i < SCREEN_COLS; i++) {
    if (line_ended) {
      write_char(x + i, y, ' ', 7);
      continue;
    }
    char c = *(s+i);
    if (c == 0) {
      line_ended = true;
      c = ' ';
    }
    write_char(x + i, y, c, 7);
  }
}

void screen_init() {
  clear_screen();
  write_line(0, 0, "DoomOS loading...");
  kprint("[SCREEN] OK\r\n");
}

void screen_refresh() {
  for (uint16_t i = 0; i < max(terminal_line_count, SCREEN_ROWS); i++) {
    char *line = (char*)terminal_get_line(i);
    write_line(0, SCREEN_ROWS - 1 - i, line);
  }
}