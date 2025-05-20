TARGET_ARCH=x86_64
CC=$(TARGET_ARCH)-elf-gcc-9.4.0/bin/$(TARGET_ARCH)-elf-gcc
STRIP=$(TARGET_ARCH)-elf-gcc-9.4.0/bin/$(TARGET_ARCH)-elf-strip

jOSh.iso: grubiso/boot/jOShload.elf grubiso/boot/jOSh.elf grubiso/boot/grub/grub.cfg
	grub-mkrescue -o jOSh.iso grubiso

grubiso/boot/jOSh.elf: os.elf
	mkdir -p grubiso/boot/
	cp os.elf grubiso/boot/jOSh.elf

grubiso/boot/jOShload.elf: module_loader/loader.elf
	mkdir -p grubiso/boot/
	cp module_loader/loader.elf grubiso/boot/jOShload.elf

grubiso/boot/grub/grub.cfg: grub.cfg
	mkdir -p grubiso/boot/grub/
	cp grub.cfg grubiso/boot/grub/grub.cfg

OBJS=\
os.o \
terminal/tty.o \
panic.o

FORCE: ;

%.o: %.c
	$(CC) -c '$<' -o '$@' -ffreestanding -O2 -Wall -std=gnu99

os.elf: linker.ld $(OBJS)
	$(CC) -T '$<' -o '$@' -ffreestanding -O2 -nostdlib $(filter-out $<,$^)

module_loader/loader.elf: FORCE
	$(MAKE) -C $(@D) "$(notdir $@)" CC=$(abspath $(CC)) STRIP=$(abspath $(STRIP)) TARGET_ARCH=$(TARGET_ARCH)

.PHONY: test
test: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -s -S

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f ./os.elf
	rm -rf ./grubiso/
	rm -f jOSh.iso
	$(MAKE) -C ./module_loader clean
