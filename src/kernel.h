#if !defined(__KERNEL_H__)
#define __KERNEL_H__
#include <stdint.h>

uint32_t ticks();
void msleep(uint32_t);
void kprint(char *);
void kprintf(char *, ...);
uint32_t min(uint32_t, uint32_t);
uint32_t max(uint32_t, uint32_t);
#endif