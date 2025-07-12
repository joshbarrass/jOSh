KERNEL_DIR?=kernel
TARGET_ARCH?=x86_64
SYSROOT?=sysroot
SYSROOT:=$(abspath $(SYSROOT))
CC=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-gcc)
CXX=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-g++)
STRIP=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-strip)

CFLAGS?=-O2 -Wall -std=gnu99
CFLAGS:=$(CFLAGS) --sysroot=$(SYSROOT)

all: jOSh.iso

include $(KERNEL_DIR)/Makefile

jOSh.iso: grubiso/boot/jOSh.elf grubiso/boot/grub/grub.cfg $(ARCH_ISO_DEPENDS)
	grub-mkrescue -o jOSh.iso grubiso

grubiso/boot/jOSh.elf: kernel/kernel.elf
	mkdir -p grubiso/boot/
	cp kernel/kernel.elf grubiso/boot/jOSh.elf

grubiso/boot/grub/grub.cfg: $(ARCH_DIR)/grub.cfg
	mkdir -p grubiso/boot/grub/
	cp $(ARCH_DIR)/grub.cfg grubiso/boot/grub/grub.cfg

.PHONY: test
test: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -s -S

.PHONY: clean
clean: arch_clean kernel_clean
	rm -f jOSh.iso
	rm -rf ./grubiso/
	rm -rf ./sysroot

.PHONY: headers
headers: kernel/install-headers
