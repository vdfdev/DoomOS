#if !defined(__INTERRUPT_H__)
#define __INTERRUPT_H__
#include <stdbool.h>

bool interrupt_is_enabled();
void interrupt_init();

#endif