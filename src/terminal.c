#define TERMINAL_MAX_LINES 200
#define TERMINAL_MAX_LEN 81

#include "terminal.h"
#include "kernel.h"

uint32_t terminal_line_count = 0;
uint32_t terminal_next_line = 0;
uint32_t terminal_next_col = 0;
char terminal_empty_string = 0;
char terminal_buffer[TERMINAL_MAX_LINES][TERMINAL_MAX_LEN];

void terminal_new_line() {
  terminal_next_col = 0;
  terminal_line_count = min(terminal_line_count + 1, TERMINAL_MAX_LINES);
  terminal_next_line = (terminal_next_line + 1) % TERMINAL_MAX_LINES;
  terminal_buffer[terminal_next_line][0] = 0;
}

void terminal_putchar(char c) {
  switch (c) {
    case '\0': return;
    case '\r': return;
    case '\n': terminal_new_line(); return;
  }
  if (terminal_next_col == TERMINAL_MAX_LEN - 2) {
    c = '+';
  }
  terminal_buffer[terminal_next_line][terminal_next_col] = c;
  terminal_buffer[terminal_next_line][terminal_next_col+1] = 0;
  terminal_next_col = min(terminal_next_col + 1, TERMINAL_MAX_LEN - 2);
}

char* terminal_get_line(uint32_t i) {
  if (i >= terminal_line_count) {
    return &terminal_empty_string;
  }
  uint16_t line_number = (terminal_next_line - 1 - i) % TERMINAL_MAX_LINES;
  char *s = terminal_buffer[line_number];
  return s;
}