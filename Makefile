KERNEL_DIR?=kernel
LIBC_DIR?=libc
TARGET_ARCH?=x86_64
SYSROOT?=sysroot
SYSROOT:=$(abspath $(SYSROOT))
CC=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-gcc)
CXX=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-g++)
STRIP=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-strip)
AR=$(abspath $(TARGET_ARCH)-elf-gcc-13.3.0/bin/$(TARGET_ARCH)-elf-ar)

CFLAGS?=-O2 -Wall -std=gnu99
CFLAGS:=$(CFLAGS) --sysroot=$(SYSROOT)

all: jOSh.iso

include $(KERNEL_DIR)/Makefile
include $(LIBC_DIR)/Makefile

jOSh.iso: grubiso/boot/jOSh.elf grubiso/boot/grub/grub.cfg $(KERNEL_ARCH_ISO_DEPENDS)
	grub-mkrescue -o jOSh.iso grubiso

grubiso/boot/jOSh.elf: kernel/kernel.elf
	mkdir -p grubiso/boot/
	cp kernel/kernel.elf grubiso/boot/jOSh.elf

grubiso/boot/grub/grub.cfg: $(KERNEL_ARCH_DIR)/grub.cfg
	mkdir -p grubiso/boot/grub/
	cp $(KERNEL_ARCH_DIR)/grub.cfg grubiso/boot/grub/grub.cfg

.PHONY: install-headers
install-headers: kernel_install-headers

.PHONY: test
test: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -s -S

.PHONY: clean
clean: kernel_arch_clean kernel_clean libc_clean
	rm -f jOSh.iso
	rm -rf ./grubiso/
	rm -rf ./sysroot

