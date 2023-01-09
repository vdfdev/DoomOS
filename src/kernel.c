#include "uart.h"
#include "pit.h"
#include <stdint.h>
#include <stdarg.h>

void msleep(uint32_t t) {
  pit_wait(t);
}

uint32_t ticks() {
  return pit_get_ticks();
}

void kputc(char c) {
  uart_putchar(c);
}

void kprint(char * s) {
  for (uint32_t i=0; s[i] != 0; i++) {
    kputc(s[i]);
  }
}

void printf_impl_d_rec(int d) {
  if (d == 0) {
    return;
  }
  printf_impl_d_rec(d/10);
  kputc((d % 10) + '0');
}

void printf_impl_d(int d) {
  if (d < 0) {
    kputc('-');
    d*=-1;
  }
  if (d == 0) {
    kputc('0');
  }
  printf_impl_d_rec(d);
}

void printf_impl_u_rec(uint32_t u) {
  if (u == 0) {
    return;
  }
  printf_impl_u_rec(u/10);
  kputc((u % 10) + '0');
}

void printf_impl_u(uint32_t u) {
  if (u==0) {
    kputc('0');
  }
  printf_impl_u_rec(u);
}

void printf_impl_x_rec(uint32_t x) {
  if (x==0) {
    return;
  }
  printf_impl_x_rec(x/16);
  uint8_t digit = x % 16; 
  uint8_t offset = '0';
  if (digit >= 10) {
    offset = 'A';
    digit -= 10;
  }
  kputc(digit + offset);
}

void printf_impl_x(uint32_t x) {
  if (x==0) {
    kputc('0');
  }
  printf_impl_x_rec(x);
}

void printf_impl(char* format, va_list args) {
  for (uint32_t i=0; format[i] != 0; i++) {
    char prevc = 0;
    if (i>0) {
      prevc = format[i-1];
    }
    char c = format[i];
    if (c == '%' && prevc != '%') {
      continue;
    }
    if (prevc != '%') {
      kputc(c);
      continue;
    }
    switch (c) {
        case 'u': // Unsgined Integer
          uint32_t u = va_arg(args, uint32_t);
          printf_impl_u(u);
          continue;
        case 'd': // Integer
          int d = va_arg(args, int);
          printf_impl_d(d);
          continue;
        case 'c': // Character
          char ch = va_arg(args, int);
          kputc(ch);
          continue;
        case 's': // String
          char* s = va_arg(args, char*);
          kprint(s);
          continue;
        case 'x': // Hex
          uint32_t x = va_arg(args, uint32_t);
          printf_impl_x(x);
          continue;
        default:
        case '%': // Escaped %
          kputc(c);
          continue;
    }
  }
}

void kprintf(char * format, ...) {
  va_list args;
  va_start(args, format);
  printf_impl(format, args);
  va_end(args);
}