BOOT_FN=PMLAUNCH
BOOT_EXT=BIN

CC=i386-elf-gcc-9.4.0/bin/i386-elf-gcc

jOSh.iso: grubiso/boot/jOSh.elf grubiso/boot/grub/grub.cfg
	grub-mkrescue -o jOSh.iso grubiso

grubiso/boot/jOSh.elf: os.elf
	mkdir -p grubiso/boot/
	cp os.elf grubiso/boot/jOSh.elf

grubiso/boot/grub/grub.cfg: grub.cfg
	mkdir -p grubiso/boot/grub/
	cp grub.cfg grubiso/boot/grub/grub.cfg

os.o: os.c
	$(CC) -c os.c -o os.o -ffreestanding -O2 -Wall -std=gnu99

bootstrap.o: bootstrap.nasm
	nasm -f elf -o bootstrap.o bootstrap.nasm

os.elf: linker.ld os.o bootstrap.o
	$(CC) -T linker.ld -o os.elf -ffreestanding -O2 -nostdlib $(filter-out $<,$^)
	grub-file --is-x86-multiboot os.elf

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
	rm -rf ./grubiso/
	rm -f jOSh.iso
	$(MAKE) -C ./jBoot clean
