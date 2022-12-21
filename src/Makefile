kernel := build/DoomOS.bin
CROSS_GCC := ./compiler/bin/i686-elf-gcc
CROSS_AS := ./compiler/bin/i686-elf-as

QEMU_FLAGS  = -kernel $(kernel) -vga std -serial stdio
QEMU_GDB    = -gdb tcp::1234 -S -m 128 -no-reboot -no-shutdown -d int,cpu_reset
#QEMU_NOX    = -nographic

c_source_files := $(wildcard *.c)
c_object_files := $(patsubst %.c, build/%.o, $(c_source_files))

all: qemu

clean:
	rm -rf build/

qemu: $(kernel)
	qemu-system-i386 $(QEMU_FLAGS) $(QEMU_NOX)

qemu-gdb: $(kernel)
	qemu-system-i386 $(QEMU_FLAGS) $(QEMU_NOX) $(QEMU_GDB)

iso: build/DoomOS.iso
	@echo "Done"

build/DoomOS.iso: $(kernel) grub.cfg
	@mkdir -p build/isofiles/boot/grub
	cp $(kernel) build/isofiles/boot/DoomOS.bin
	cp grub.cfg build/isofiles/boot/grub
	grub-mkrescue -o build/DoomOS.iso build/isofiles
	@rm -r build/isofiles

$(kernel): $(c_object_files) build/boot.o
	$(CROSS_GCC) -T linker.ld -o $(kernel) -ffreestanding -O2 -nostdlib build/boot.o $(c_object_files) -lgcc

build/boot.o: boot.S
	@mkdir -p $(shell dirname $@)
	$(CROSS_AS) boot.S -o build/boot.o

build/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CROSS_GCC) -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c $< -o $@

