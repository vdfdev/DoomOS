#if !defined(__PIT_H__)
#define __PIT_H__
#include <stdint.h>

void pit_init();
void pit_tick();
void pit_wait(uint32_t);
uint32_t pit_get_ticks();

#endif