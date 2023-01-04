#include "kernel.h"
#include "asm.h"

#define IDT_MAX_DESCRIPTORS 256
#define IDT_DEFAULT_ATTR 0x8E

struct interrupt_idt_entry_t {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed));

struct interrupt_idtr_t {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed));

__attribute__((aligned(0x10))) 
static struct interrupt_idt_entry_t interrupt_idt[IDT_MAX_DESCRIPTORS]; // Create an array of IDT entries; aligned for performance
static struct interrupt_idtr_t interrupt_idtr;

void interrupt_idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    struct interrupt_idt_entry_t* descriptor = &interrupt_idt[vector];
 
    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

#define FATAL_EXCEPTION_HANDLER(i, msg) \
  static void interrupt_exception##i() {\
    kprintf("EXCEPTION %u: %s\n", i, msg);\
    asm_halt();\
  }

FATAL_EXCEPTION_HANDLER(0, "Divide error")
FATAL_EXCEPTION_HANDLER(1, "Debug exception")
FATAL_EXCEPTION_HANDLER(2, "Unknown error")
FATAL_EXCEPTION_HANDLER(3, "Breakpoint")
FATAL_EXCEPTION_HANDLER(4, "Overflow")
FATAL_EXCEPTION_HANDLER(5, "Bounds check")
FATAL_EXCEPTION_HANDLER(6, "Illegal instruction")
FATAL_EXCEPTION_HANDLER(7, "FPU exception")
FATAL_EXCEPTION_HANDLER(8, "Double fault")
FATAL_EXCEPTION_HANDLER(9, "Coprocessor segment overrun")
FATAL_EXCEPTION_HANDLER(10, "Invalid TSS")
FATAL_EXCEPTION_HANDLER(11, "Segment not present")
FATAL_EXCEPTION_HANDLER(12, "Stack exception")
FATAL_EXCEPTION_HANDLER(13, "General protection fault")
FATAL_EXCEPTION_HANDLER(14, "Page fault")
FATAL_EXCEPTION_HANDLER(15, "Unknown error")
FATAL_EXCEPTION_HANDLER(16, "Coprocessor error")

void interrupt_init_idt() {
  for (uint16_t i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
    interrupt_idt[i].isr_low = 0;
    interrupt_idt[i].kernel_cs = 0;
    interrupt_idt[i].reserved = 0;
    interrupt_idt[i].attributes = 0;
    interrupt_idt[i].isr_high = 0;
  }
}

void interrupt_init() {
  interrupt_init_idt();
  interrupt_idtr.base = (uint32_t)&interrupt_idt[0];
  interrupt_idtr.limit = (uint16_t)(sizeof(struct interrupt_idt_entry_t) * IDT_MAX_DESCRIPTORS - 1); 
  interrupt_idt_set_descriptor(0, interrupt_exception0, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(1, interrupt_exception1, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(2, interrupt_exception2, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(3, interrupt_exception3, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(4, interrupt_exception4, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(5, interrupt_exception5, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(6, interrupt_exception6, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(7, interrupt_exception7, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(8, interrupt_exception8, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(9, interrupt_exception9, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(10, interrupt_exception10, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(11, interrupt_exception11, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(12, interrupt_exception12, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(13, interrupt_exception13, IDT_DEFAULT_ATTR);
  interrupt_idt_set_descriptor(14, interrupt_exception14, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(15, interrupt_exception15, IDT_DEFAULT_ATTR); 
  interrupt_idt_set_descriptor(16, interrupt_exception16, IDT_DEFAULT_ATTR);
 
  __asm__ volatile ("lidt %0" : : "m"(interrupt_idtr)); // load the new IDT
  __asm__ volatile ("sti"); // set the interrupt flag  
  kprint("[INTERRUPTS] OK\n");
}