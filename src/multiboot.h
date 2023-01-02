#if !defined(__MULTIBOOT_H__)
#define __MULTIBOOT_H__
#include <stdint.h>
#include <stdbool.h>

/*
https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

        +-------------------+
0       | flags             |    (required)
        +-------------------+
4       | mem_lower         |    (present if flags[0] is set)
8       | mem_upper         |    (present if flags[0] is set)
        +-------------------+
12      | boot_device       |    (present if flags[1] is set)
        +-------------------+
16      | cmdline           |    (present if flags[2] is set)
        +-------------------+
20      | mods_count        |    (present if flags[3] is set)
24      | mods_addr         |    (present if flags[3] is set)
        +-------------------+
28 - 40 | syms              |    (present if flags[4] or
        |                   |                flags[5] is set)
        +-------------------+
44      | mmap_length       |    (present if flags[6] is set)
48      | mmap_addr         |    (present if flags[6] is set)
        +-------------------+
52      | drives_length     |    (present if flags[7] is set)
56      | drives_addr       |    (present if flags[7] is set)
        +-------------------+
60      | config_table      |    (present if flags[8] is set)
        +-------------------+
64      | boot_loader_name  |    (present if flags[9] is set)
        +-------------------+
68      | apm_table         |    (present if flags[10] is set)
        +-------------------+
72      | vbe_control_info  |    (present if flags[11] is set)
76      | vbe_mode_info     |
80      | vbe_mode          |
82      | vbe_interface_seg |
84      | vbe_interface_off |
86      | vbe_interface_len |
        +-------------------+
88      | framebuffer_addr  |    (present if flags[12] is set)
96      | framebuffer_pitch |
100     | framebuffer_width |
104     | framebuffer_height|
108     | framebuffer_bpp   |
109     | framebuffer_type  |
110-115 | color_info        |
        +-------------------+
*/
struct multiboot_flags_t {
  bool has_mem : 1;
  bool has_boot_device : 1;
  bool has_cmdline : 1;
  bool has_mods : 1;
  bool has_syms_a_out : 1;
  bool has_syms_elf : 1;
  bool has_mmap : 1;
  bool has_drives : 1;
  bool has_config_table : 1;
  bool has_boot_loader_name : 1;
  bool has_apm_table : 1;
  bool has_vbe : 1;
  bool has_framebuffer : 1;
} __attribute__((packed));

struct multiboot_mem_t {
  uint32_t mem_lower;
  uint32_t mem_upper;
} __attribute__((packed));

struct multiboot_boot_device_t {
  uint8_t part3;
  uint8_t part2;
  uint8_t part1;
  uint8_t drive;
} __attribute__((packed));

struct multiboot_mods_entry_t {
  uint16_t start;
  uint16_t end;
  char str[8];
} __attribute__((packed));

struct multiboot_mods_t {
  uint32_t count;
  struct multiboot_mods_entry_t* addr;
} __attribute__((packed));

struct multiboot_syms_a_out_t {
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
} __attribute__((packed));

struct multiboot_syms_elf_t {
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} __attribute__((packed));

struct multiboot_mmap_t {
  uint32_t length;
  void* addr;
} __attribute__((packed));

struct multiboot_drives_t {
  uint32_t length;
  void* addr;
} __attribute__((packed));

struct multiboot_config_tables_t {
  void* addr;
} __attribute__((packed));

struct multiboot_apm_table_t {
  void* addr;
} __attribute__((packed));

struct multiboot_vbe_t {
  void* control_info;
  void* mode_info;
  uint16_t mode;
  uint16_t interface_seg;
  uint16_t interface_off;
  uint16_t interface_len;
} __attribute__((packed));

struct multiboot_framebuffer_t {
  void* addr;
  void* addr_64bit;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint32_t bpp;
  bool type : 1;
  // Missing last fields because it depends on the types.
  // Add later if we need it.
} __attribute__((packed));

void multiboot_init(void *);

#endif