TARGET_ARCH?=x86_64
ARCH_DIR=kernel/arch/$(TARGET_ARCH)
SYSROOT?=sysroot
CC=$(abspath $(TARGET_ARCH)-elf-gcc-9.4.0/bin/$(TARGET_ARCH)-elf-gcc)
STRIP=$(abspath $(TARGET_ARCH)-elf-gcc-9.4.0/bin/$(TARGET_ARCH)-elf-strip)

CFLAGS?=-O2 -Wall -std=gnu99

SYSROOT:=$(abspath $(SYSROOT))
CFLAGS:=$(CFLAGS) --sysroot=$(SYSROOT)

all: jOSh.iso

include $(ARCH_DIR)/make.config

jOSh.iso: grubiso/boot/jOSh.elf grubiso/boot/grub/grub.cfg $(ARCH_ISO_DEPENDS)
	grub-mkrescue -o jOSh.iso grubiso

grubiso/boot/jOSh.elf: kernel/kernel.elf
	mkdir -p grubiso/boot/
	cp kernel/kernel.elf grubiso/boot/jOSh.elf

grubiso/boot/grub/grub.cfg: $(ARCH_DIR)/grub.cfg
	mkdir -p grubiso/boot/grub/
	cp $(ARCH_DIR)/grub.cfg grubiso/boot/grub/grub.cfg

FORCE: ;

kernel/install-headers: FORCE
	$(MAKE) -C kernel install-headers TARGET_ARCH="$(TARGET_ARCH)" CC="$(CC)" STRIP="$(STRIP)" CFLAGS="$(CFLAGS)" SYSROOT="$(SYSROOT)"

kernel/%: headers FORCE
	$(MAKE) -C kernel $* TARGET_ARCH="$(TARGET_ARCH)" CC="$(CC)" STRIP="$(STRIP)" CFLAGS="$(CFLAGS)" SYSROOT="$(SYSROOT)"

.PHONY: test
test: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -s -S

.PHONY: clean
clean: arch_clean
	rm -f jOSh.iso
	rm -rf ./grubiso/
	$(MAKE) -C kernel clean TARGET_ARCH="$(TARGET_ARCH)" CC="$(CC)"
	rm -rf ./sysroot

.PHONY: headers
headers: kernel/install-headers
