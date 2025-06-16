TARGET_ARCH?=x86_64
ARCH_DIR=kernel/arch/$(TARGET_ARCH)
CC=$(TARGET_ARCH)-elf-gcc-9.4.0/bin/$(TARGET_ARCH)-elf-gcc
STRIP=$(TARGET_ARCH)-elf-gcc-9.4.0/bin/$(TARGET_ARCH)-elf-strip

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

OBJS=\
kernel/kernel.o

FORCE: ;

kernel/%.o: kernel/%.c
	$(CC) -c '$<' -o '$@' -ffreestanding -O2 -Wall -std=gnu99

kernel/kernel.elf: $(ARCH_DIR)/linker.ld $(OBJS)
	$(CC) -T '$<' -o '$@' -ffreestanding -O2 -nostdlib $(filter-out $<,$^)

.PHONY: test
test: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -s -S

.PHONY: clean
clean: arch_clean
	rm -f $(OBJS)
	rm -f ./kernel/kernel.elf
	rm -rf ./grubiso/
	rm -f jOSh.iso
