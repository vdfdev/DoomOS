#define TERMINAL_MAX_LINES 200
#define TERMINAL_MAX_LEN 81

#include "terminal.h"
#include "kernel.h"

volatile uint32_t terminal_line_count = 0;
volatile uint32_t terminal_next_line = 0;
volatile uint32_t terminal_next_col = 0;
volatile char terminal_empty_string = 0;
volatile char terminal_buffer[TERMINAL_MAX_LINES][TERMINAL_MAX_LEN];
volatile char terminal_cmd[TERMINAL_MAX_LEN];
volatile uint16_t terminal_cmd_len = 0;

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

void terminal_exec(char* s) {
  kprintf("EXEC '%s'\r\n", s);
}

void terminal_rxchar(char c) { 
  if (c == '\r') {
    kprint("\r\n");
    terminal_exec((char*)terminal_cmd);
    kprint("$ ");
    terminal_cmd[0] = 0;
    terminal_cmd_len = 0;
    return;
  }
  if (terminal_cmd_len >= TERMINAL_MAX_LEN -1) {
    return;
  }
  kprintf("%c", c);
  terminal_cmd[terminal_cmd_len] = c;
  terminal_cmd[terminal_cmd_len+1] = 0;
  terminal_cmd_len++;
}

char* terminal_get_line(uint32_t i) {
  if (i >= terminal_line_count) {
    return (char*)&terminal_empty_string;
  }
  uint16_t line_number = (terminal_next_line - i) % TERMINAL_MAX_LINES;
  char *s = (char*)terminal_buffer[line_number];
  return s;
}