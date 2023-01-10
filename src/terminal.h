#ifndef __TERMINAL_H__
#define __TERMINAL_H__
#include <stdint.h>

extern uint32_t terminal_line_count;
void terminal_putchar(char);
char* terminal_get_line(uint32_t i);

#endif