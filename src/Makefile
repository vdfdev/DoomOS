kernel := build/kernel.bin
iso := build/DoomOS.iso
linker_script := linker.ld
grub_cfg := grub.cfg

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O1 -Wall -MD -ggdb -m32 -fno-omit-frame-pointer -Werror -nostdlib

QEMU_FLAGS  = -cdrom $(iso) -vga std -serial stdio
QEMU_GDB    = -gdb tcp::1234 -S -m 128 -no-reboot -no-shutdown -d int,cpu_reset
# QEMU_CURSES = -curses
# QEMU_NOX    = -nographic

assembly_source_files := $(wildcard *.asm)
c_source_files := $(wildcard *.c)
assembly_object_files := $(patsubst %.asm, build/%.o, $(assembly_source_files))
c_object_files := $(patsubst %.c, build/%.o, $(c_source_files))

# regardless of the state of files, make will always run these targets
.PHONY: all clean qemu qemu-gdb qemu-nox qemu-gdb-nox iso kernel

#default target (the first one in the Makefile)
all: $(iso)

# remove all "compile-time-generated" files.
clean:
	rm -rf build


# CONVENIENT TARGETS TO RUN QEMU WITH DIFFERENT OPTIONS
# basic run
qemu: $(iso)
	qemu-system-x86_64 $(QEMU_FLAGS) $(QEMU_CURSES) $(QEMU_NOX)

# run with debug options
qemu-gdb: $(iso)
	qemu-system-x86_64 $(QEMU_FLAGS) $(QEMU_CURSES) $(QEMU_NOX) $(QEMU_GDB)


# TARGETS THAT ACTUALLY BUILD NEW FILES
# target "iso" will print "Done" once the prerequisites are fulfilled
iso: $(iso)
	@echo "Done"

# create file defined in variable "iso"
$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	cp $(kernel) build/isofiles/boot/kernel.bin
	cp $(grub_cfg) build/isofiles/boot/grub
	grub-mkrescue -o $(iso) build/isofiles #2> /dev/null
	@rm -r build/isofiles

# create file defined in variable "kernel"
$(kernel): $(c_object_files) $(assembly_object_files) $(linker_script)
	ld -m elf_i386  -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files)

# compile any C file
build/%.o: %.c
	@mkdir -p $(shell dirname $@)
	gcc $(CFLAGS) -c $< -o $@

# compile any assembly file
build/%.o: %.asm
	@mkdir -p $(shell dirname $@)
	nasm -felf32 -g -F dwarf $< -o $@

