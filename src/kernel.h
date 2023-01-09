#if !defined(__KERNEL_H__)
#define __KERNEL_H__
#include <stdint.h>

uint32_t ticks();
void msleep(uint32_t);
void kprint(char *);
void kprintf(char *, ...);

#endif