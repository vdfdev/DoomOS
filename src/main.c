#include "uart.h"
#include "asm.h"
#include "kernel.h"
#include "screen.h"
#include "page.h"
#include "gdt.h"
#include "multiboot.h"
#include "interrupt.h"

void kernel_main(void* multiboot_info_ptr){
    uart_init();
    gdt_init(); 
    page_init();
    multiboot_init(multiboot_info_ptr);
    interrupt_init();
    screen_init();
    kprint("Welcome to DoomOS!\r\n");
    for (;;) {
        asm_halt();
    }
}
