#include "uart.h"

void printk(char * s) {
    uart_print(s);
}