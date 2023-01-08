#include "multiboot.h"
#include "kernel.h"

// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

struct multiboot_flags_t* multiboot_flags = 0;
struct multiboot_mem_t* multiboot_mem = 0;
struct multiboot_boot_device_t* multiboot_boot_device = 0;
char* multiboot_cmdline = 0;
struct multiboot_mods_t* multiboot_mods = 0;
struct multiboot_syms_a_out_t* multiboot_syms_a_out = 0;
struct multiboot_syms_elf_t* multiboot_syms_elf = 0;
struct multiboot_mmap_t* multiboot_mmap = 0;
struct multiboot_drives_t* multiboot_drives = 0;
struct multiboot_config_tables_t* multiboot_config_tables = 0;
char* multiboot_boot_loader_name = 0;
struct multiboot_apm_table_t* multiboot_apm_table = 0;
struct multiboot_vbe_t* multiboot_vbe = 0;
struct multiboot_framebuffer_t* multiboot_framebuffer = 0;

void multiboot_init(void * multiboot_info_ptr) {
  uint8_t* base_ptr = multiboot_info_ptr;
  multiboot_flags = multiboot_info_ptr;
  kprintf("[MULTIBOOT] multiboot_info_ptr: 0x%x\r\n", multiboot_info_ptr); 
  if (multiboot_flags->has_mem) {
    multiboot_mem = (void*)(base_ptr + 4);
    kprintf("[MULTIBOOT] multiboot_mem: 0x%x mem_lower: 0x%x mem_uper: 0x%x\r\n", multiboot_mem, multiboot_mem->mem_lower, multiboot_mem->mem_upper);
  }
  if (multiboot_flags->has_boot_device) {
    multiboot_boot_device = (void*)(base_ptr + 12);
    kprintf("[MULTIBOOT] multiboot_boot_device: 0x%x bootdevice drive: 0x%x part1: 0x%x part2: 0x%x part3: 0x%x\r\n", multiboot_boot_device, multiboot_boot_device->drive, multiboot_boot_device->part1, multiboot_boot_device->part2, multiboot_boot_device->part3);
  }
  if (multiboot_flags->has_cmdline) {
    multiboot_cmdline = (void*)(base_ptr + 16);
    kprintf("[MULTIBOOT] multiboot_cmdline: 0x%x value: %s\r\n", multiboot_cmdline, multiboot_cmdline);
  }
  if (multiboot_flags->has_mods) {
    multiboot_mods = (void*)(base_ptr + 20);
    kprintf("[MULTIBOOT] multiboot_mods: 0x%x count: %u\r\n", multiboot_mods, multiboot_mods->count);
  }
  if (multiboot_flags->has_syms_a_out) {
    multiboot_syms_a_out = (void*)(base_ptr + 28);
    kprintf("[MULTIBOOT] multiboot_syms_a_out: 0x%x tabsize: %u strsize: %u addr: %x\r\n", multiboot_syms_a_out, multiboot_syms_a_out->tabsize, multiboot_syms_a_out->strsize, multiboot_syms_a_out->addr);
  }
  if (multiboot_flags->has_syms_elf) {
    multiboot_syms_elf = (void*)(base_ptr + 28);
    kprintf("[MULTIBOOT] multiboot_syms_elf: 0x%x num: %u size: %u addr: %x shndx: %u\r\n", multiboot_syms_elf, multiboot_syms_elf->num, multiboot_syms_elf->size, multiboot_syms_elf->addr, multiboot_syms_elf->shndx);
  }
  if (multiboot_flags->has_mmap) {
    multiboot_mmap = (void*)(base_ptr + 44);
    kprintf("[MULTIBOOT] multiboot_mmap: 0x%x length: %u addr: 0x%x\r\n", multiboot_mmap, multiboot_mmap->length, multiboot_mmap->addr);
  }
  if (multiboot_flags->has_drives) {
    multiboot_drives = (void*)(base_ptr + 52);
    kprintf("[MULTIBOOT] multiboot_drives: 0x%x length: %u addr: 0x%x\r\n", multiboot_drives, multiboot_drives->length, multiboot_drives->addr);
  }
  if (multiboot_flags->has_config_table) {
    multiboot_config_tables = (void*)(base_ptr + 60);
    kprintf("[MULTIBOOT] multiboot_config_tables: 0x%x addr: 0x%x\r\n", multiboot_config_tables, multiboot_config_tables->addr);
  }
  if (multiboot_flags->has_boot_loader_name) {
    multiboot_boot_loader_name = (void*)(base_ptr + 64);
    kprintf("[MULTIBOOT] multiboot_boot_loader_name: 0x%x value: %s\r\n", multiboot_boot_loader_name, multiboot_boot_loader_name);
  }
  if (multiboot_flags->has_apm_table) {
    multiboot_apm_table = (void*)(base_ptr + 68);
    kprintf("[MULTIBOOT] multiboot_apm_table: 0x%x addr: 0x%x\r\n", multiboot_apm_table, multiboot_apm_table->addr);
  }
  if (multiboot_flags->has_vbe) {
    multiboot_vbe = (void*)(base_ptr + 72);
    kprintf("[MULTIBOOT] multiboot_vbe: 0x%x control_info: 0x%x mode_info: 0x%x mode: 0x%x interface_seg: 0x%x interface_off: 0x%x interface_len: %u\r\n", multiboot_vbe, multiboot_vbe->control_info, multiboot_vbe->mode_info, multiboot_vbe->mode, multiboot_vbe->interface_seg, multiboot_vbe->interface_off, multiboot_vbe->interface_len);
  }
  if (multiboot_flags->has_framebuffer) {
    multiboot_framebuffer = (void*)(base_ptr + 88);
    kprintf("[MULTIBOOT] multiboot_framebuffer: 0x%x addr: 0x%x addr_64bit: %x pitch: %u width: %u height: %u bpp: %u type: %u\r\n", multiboot_framebuffer, multiboot_framebuffer->addr, multiboot_framebuffer->addr_64bit, multiboot_framebuffer->pitch, multiboot_framebuffer->width, multiboot_framebuffer->height, multiboot_framebuffer->bpp, multiboot_framebuffer->type);
  }
  kprint("[MULTIBOOT] OK\r\n");
}