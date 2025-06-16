ARCH_DIR=arch/$(TARGET_ARCH)

include $(ARCH_DIR)/make.config

OBJS=\
kernel.o

FORCE: ;

%.o: %.c
	$(CC) -c '$<' -o '$@' -ffreestanding $(CFLAGS)

kernel.elf: $(ARCH_DIR)/linker.ld $(OBJS)
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(filter-out $<,$^)

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
