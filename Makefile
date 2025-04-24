BOOT_FN=PMLAUNCH
BOOT_EXT=BIN

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
os.o

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

# custom bootloader stuff -- currently not usable
boot.img: ./jBoot/boot.img pmlaunch.bin
	cp ./jBoot/boot.img ./boot.img
	mcopy -i boot.img ./pmlaunch.bin ::PMLAUNCH.BIN

pmlaunch.bin: pmlaunch.nasm ./jBoot/bsect.h ./jBoot/bsect.nasm
	nasm -f bin -o pmlaunch.bin -l pmlaunch.lst pmlaunch.nasm

jBoot/%: FORCE
	$(MAKE) -C $(@D) "$(notdir $@)" BOOT_FN=$(BOOT_FN) BOOT_EXT=$(BOOT_EXT)

.PHONY: clean
clean:
	rm -f ./boot.img
	rm -f ./pmlaunch.bin ./pmlaunch.lst
	rm -f $(OBJS)
	rm -f ./os.elf
	rm -rf ./grubiso/
	rm -f jOSh.iso
	$(MAKE) -C ./jBoot clean
	$(MAKE) -C ./module_loader clean
