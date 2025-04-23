BOOT_FN=PMLAUNCH
BOOT_EXT=BIN

CC=i386-elf-gcc-9.4.0/bin/i386-elf-gcc

jOSh.iso: grubiso/boot/jOShload.elf grubiso/boot/jOSh.elf grubiso/boot/grub/grub.cfg
	grub-mkrescue -o jOSh.iso grubiso

grubiso/boot/jOSh.elf: os.o
	mkdir -p grubiso/boot/
	cp os.o grubiso/boot/jOSh.elf

grubiso/boot/jOShload.elf: loader.elf
	mkdir -p grubiso/boot/
	cp loader.elf grubiso/boot/jOShload.elf

grubiso/boot/grub/grub.cfg: grub.cfg
	mkdir -p grubiso/boot/grub/
	cp grub.cfg grubiso/boot/grub/grub.cfg

os.o: os.c
	$(CC) -c os.c -o os.o -ffreestanding -O2 -Wall -std=gnu99

module_loader.o: module_loader.c
	$(CC) -c module_loader.c -o module_loader.o -ffreestanding -O2 -Wall -std=gnu99

bootstrap.o: bootstrap.nasm
	nasm -f elf -o bootstrap.o bootstrap.nasm

loader.elf: linker.ld bootstrap.o module_loader.o
	$(CC) -T linker.ld -o loader.elf -ffreestanding -O2 -nostdlib $(filter-out $<,$^)
	grub-file --is-x86-multiboot loader.elf

.PHONY: test
test: jOSh.iso
	qemu-system-i386 -cdrom jOSh.iso -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-i386 -cdrom jOSh.iso -s -S

# custom bootloader stuff -- currently not usable
boot.img: ./jBoot/boot.img pmlaunch.bin
	cp ./jBoot/boot.img ./boot.img
	mcopy -i boot.img ./pmlaunch.bin ::PMLAUNCH.BIN

pmlaunch.bin: pmlaunch.nasm ./jBoot/bsect.h ./jBoot/bsect.nasm
	nasm -f bin -o pmlaunch.bin -l pmlaunch.lst pmlaunch.nasm

FORCE: ;
jBoot/%: FORCE
	$(MAKE) -C $(@D) "$(notdir $@)" BOOT_FN=$(BOOT_FN) BOOT_EXT=$(BOOT_EXT)

.PHONY: clean
clean:
	rm -f ./boot.img
	rm -f ./pmlaunch.bin ./pmlaunch.lst
	rm -f ./os.o
	rm -f ./bootstrap.o
	rm -f ./os.elf
	rm -f ./module_loader.o
	rm -f ./loader.elf
	rm -rf ./grubiso/
	rm -f jOSh.iso
	$(MAKE) -C ./jBoot clean
