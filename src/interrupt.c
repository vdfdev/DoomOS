#include "kernel.h"
#include "pit.h"
#include <stdint.h>
#include <stdbool.h>

#define IDT_MAX_DESCRIPTORS 256
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */
#define PIC_ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define PIC_ICW1_INIT	0x10		/* Initialization - required! */
#define PIC_ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define IRQ_OFFSET      32

static inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline __attribute__((always_inline)) void io_wait(void)
{
    outb(0x80, 0);
}

static inline __attribute__((always_inline)) uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

// See "Figure 6-2. IDT Gate Descriptors", "Interrupt gate" on intel manual.
struct interrupt_idt_entry_t {
	uint16_t    isr_low;             // The lower 16 bits of the ISR's address
	uint16_t    segment_selector;    // GDT segment selector
	uint8_t     zero0;               // Set to zero
    bool        zero1:1;             // Must be zero
    bool        one0:1;              // Must be one
    bool        one1:1;              // Must be one
    bool        D:1;                 // Size of gate, 1 = 32 bits; 0 = 16 bits;
    bool        zero2:1;             // Must be zero
    uint8_t     dpl:2;               // Descriptor privilege level
    bool        present:1;           // Present flag
	uint16_t    isr_high;            // The higher 16 bits of the ISR's address
} __attribute__((packed));

struct interrupt_idtr_t {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed));

__attribute__((aligned(0x10))) 
static struct interrupt_idt_entry_t interrupt_idt[IDT_MAX_DESCRIPTORS]; // Create an array of IDT entries; aligned for performance
static struct interrupt_idtr_t interrupt_idtr;

void interrupt_idt_set_descriptor(uint8_t vector, void* isr) {
    struct interrupt_idt_entry_t* descriptor = &interrupt_idt[vector];
 
    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->segment_selector      = 0x08;
    descriptor->zero0          = 0;
    descriptor->zero1          = 0;
    descriptor->one0           = 1;
    descriptor->one1           = 1;
    descriptor->D              = 1; // 32 bits
    descriptor->zero2          = 0;
    descriptor->dpl            = 0; // Ring 0, kernel
    descriptor->present        = 1; // Present
    descriptor->isr_high       = (uint32_t)isr >> 16;
}

void interrupt_irq_handle(uint32_t irq) {
  switch (irq) {
    case 0:
      pit_tick();
      break;
    default:
      kprintf("UNHANDLED IRQ %u\r\n", irq);
      break;
  }
  if(irq >= 8)
    outb(PIC2_COMMAND, PIC_EOI);

  outb(PIC1_COMMAND, PIC_EOI);
}

#define IRQ_HANDLER(irq) \
  void interrupt_irq##irq(); \
  asm(".globl interrupt_irq" #irq "\n" \
      "interrupt_irq" #irq ":\n" \
      "cli\n" \
      "push $" #irq "\n" \
      "call interrupt_irq_handle\n" \
      "add  $4, %esp\n" \
      "sti\n" \
      "iret\n");

#define FATAL_EXCEPTION_HANDLER(i, msg) \
  static void interrupt_exception##i() {\
    kprintf("FATAL EXCEPTION %u: %s\r\n", i, msg);\
    asm volatile("cli; hlt" ::: "memory");\
  }

FATAL_EXCEPTION_HANDLER(0, "Divide error")
FATAL_EXCEPTION_HANDLER(1, "Debug exception")
FATAL_EXCEPTION_HANDLER(2, "Nonmaskable interrupt")
FATAL_EXCEPTION_HANDLER(3, "Breakpoint")
FATAL_EXCEPTION_HANDLER(4, "Overflow")
FATAL_EXCEPTION_HANDLER(5, "Bounds check")
FATAL_EXCEPTION_HANDLER(6, "Illegal opcode")
FATAL_EXCEPTION_HANDLER(7, "FPU exception")
FATAL_EXCEPTION_HANDLER(8, "Double fault")
FATAL_EXCEPTION_HANDLER(9, "Coprocessor segment overrun")
FATAL_EXCEPTION_HANDLER(10, "Invalid TSS")
FATAL_EXCEPTION_HANDLER(11, "Segment not present")
FATAL_EXCEPTION_HANDLER(12, "Stack exception")
FATAL_EXCEPTION_HANDLER(13, "General protection fault")
FATAL_EXCEPTION_HANDLER(14, "Page fault")
FATAL_EXCEPTION_HANDLER(16, "x87 FPU error")
FATAL_EXCEPTION_HANDLER(17, "Alignment Check Exception")
FATAL_EXCEPTION_HANDLER(18, "Machine-Check Exception")
FATAL_EXCEPTION_HANDLER(19, "SIMD Floating-Point Exception")
FATAL_EXCEPTION_HANDLER(20, "Virtualization Exception")
FATAL_EXCEPTION_HANDLER(21, "Control Protection Exception")

IRQ_HANDLER(0)
IRQ_HANDLER(1)
IRQ_HANDLER(2)
IRQ_HANDLER(3)
IRQ_HANDLER(4)
IRQ_HANDLER(5)
IRQ_HANDLER(6)
IRQ_HANDLER(7)
IRQ_HANDLER(8)
IRQ_HANDLER(9)
IRQ_HANDLER(10)
IRQ_HANDLER(11)
IRQ_HANDLER(12)
IRQ_HANDLER(13)
IRQ_HANDLER(14)
IRQ_HANDLER(15)

bool interrupt_is_enabled() {
  uint32_t flags;
  asm volatile("pushf ; pop %0"
          : "=rm" (flags)
          : /* no input */
          : "memory");
  return (flags & 0x0200) > 0;
}

void interrupt_pic_remap()
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, IRQ_OFFSET);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, IRQ_OFFSET+8);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, PIC_ICW4_8086);
	io_wait();
	outb(PIC2_DATA, PIC_ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void interrupt_init_idt() {
  for (uint16_t i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
    uint32_t* idt_ptr = (uint32_t*)&interrupt_idt[i];
    *(idt_ptr) = 0;
    *(idt_ptr+1) = 0;
  }
}

void interrupt_init() {
  interrupt_init_idt();
  interrupt_idtr.base = (uint32_t)&interrupt_idt[0];
  interrupt_idtr.limit = (uint16_t)(sizeof(struct interrupt_idt_entry_t) * IDT_MAX_DESCRIPTORS - 1); 
  // Intel Reserved interrupts
  interrupt_idt_set_descriptor(0, interrupt_exception0); 
  interrupt_idt_set_descriptor(1, interrupt_exception1);
  interrupt_idt_set_descriptor(2, interrupt_exception2); 
  interrupt_idt_set_descriptor(3, interrupt_exception3);
  interrupt_idt_set_descriptor(4, interrupt_exception4); 
  interrupt_idt_set_descriptor(5, interrupt_exception5);
  interrupt_idt_set_descriptor(6, interrupt_exception6); 
  interrupt_idt_set_descriptor(7, interrupt_exception7);
  interrupt_idt_set_descriptor(8, interrupt_exception8); 
  interrupt_idt_set_descriptor(9, interrupt_exception9);
  interrupt_idt_set_descriptor(10, interrupt_exception10); 
  interrupt_idt_set_descriptor(11, interrupt_exception11);
  interrupt_idt_set_descriptor(12, interrupt_exception12); 
  interrupt_idt_set_descriptor(13, interrupt_exception13);
  interrupt_idt_set_descriptor(14, interrupt_exception14); 
  interrupt_idt_set_descriptor(16, interrupt_exception16); 
  interrupt_idt_set_descriptor(17, interrupt_exception17); 
  interrupt_idt_set_descriptor(18, interrupt_exception18); 
  interrupt_idt_set_descriptor(19, interrupt_exception19); 
  interrupt_idt_set_descriptor(20, interrupt_exception20); 
  interrupt_idt_set_descriptor(21, interrupt_exception21);
  // IRQs 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 0, interrupt_irq0);
  interrupt_idt_set_descriptor(IRQ_OFFSET + 1, interrupt_irq1); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 2, interrupt_irq2); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 3, interrupt_irq3);
  interrupt_idt_set_descriptor(IRQ_OFFSET + 4, interrupt_irq4); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 5, interrupt_irq5); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 6, interrupt_irq6); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 7, interrupt_irq7);
  interrupt_idt_set_descriptor(IRQ_OFFSET + 8, interrupt_irq8); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 9, interrupt_irq9); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 10, interrupt_irq10); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 11, interrupt_irq11);
  interrupt_idt_set_descriptor(IRQ_OFFSET + 12, interrupt_irq12); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 13, interrupt_irq13); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 14, interrupt_irq14); 
  interrupt_idt_set_descriptor(IRQ_OFFSET + 15, interrupt_irq15);
  __asm__ volatile ("lidt %0" : : "m"(interrupt_idtr)); // load the new IDT
  interrupt_pic_remap();
  __asm__ volatile ("sti"); // set the interrupt flag  
  kprint("[INTERRUPTS] OK\r\n");
}