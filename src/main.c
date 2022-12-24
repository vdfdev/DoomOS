#include "uart.h"
#include "asm.h"
#include "kernel.h"
#include "screen.h"
#include "page.h"
#include "gdt.h"

void kernel_main(void* multiboot_info_ptr, void* page_tables_ptr){
    uart_init();
    gdt_init(); 
    page_init(multiboot_info_ptr, page_tables_ptr);
    screen_init();
    printk("Welcome to DoomOS!\n");
    asm_halt();
}
