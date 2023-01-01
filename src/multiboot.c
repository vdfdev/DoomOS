#include "multiboot.h"
#include "kernel.h"

// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

struct multiboot_flags_t* multiboot_flags;
struct multiboot_mem_t* multiboot_mem;
struct multiboot_boot_device_t* multiboot_boot_device;
struct multiboot_cmdline_t* multiboot_cmdline;
struct multiboot_mods_t* multiboot_mods;
struct multiboot_syms_a_out_t* multiboot_syms_a_out;


void multiboot_init(void * multiboot_info_ptr) {
  multiboot_flags = multiboot_info_ptr; 
  if (multiboot_flags->has_mem) {
    multiboot_mem = (void*)(multiboot_flags + 1);
    kprintf("[MULTIBOOT] mem_lower: %u mem_uper: %u\n", multiboot_mem->mem_lower, multiboot_mem->mem_upper);
  }
}