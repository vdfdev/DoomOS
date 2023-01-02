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
    asm_halt();
}
