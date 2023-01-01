#include "uart.h"
#include "asm.h"
#include "kernel.h"
#include "screen.h"
#include "page.h"
#include "gdt.h"
#include "multiboot.h"

void kernel_main(void* multiboot_info_ptr, void* page_tables_ptr){
    uart_init();
    gdt_init(); 
    page_init(page_tables_ptr);
    multiboot_init(multiboot_info_ptr);
    screen_init();
    kprint("Welcome to DoomOS!\n");
    kprintf("%d: 0 %u: 0 1: %d 128: %d -128: %d 2147483647: %d -2147483648: %d perc: %% 4294967295: %u %z\n", 0, 0, 1, 128, -128, 2147483647, -2147483648, 4294967295);
    asm_halt();
}
