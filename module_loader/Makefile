module_loader.o: module_loader.c elf.h
	$(CC) -m32 -c module_loader.c -o module_loader.o -ffreestanding -O2 -Wall -std=gnu99

elf.o: elf.c elf.h
	$(CC) -m32 -c elf.c -o elf.o -ffreestanding -O2 -Wall -std=gnu99

bootstrap.o: bootstrap.nasm
	nasm -f elf32 -o bootstrap.o bootstrap.nasm

loader.elf: module_loader.ld bootstrap.o module_loader.o elf.o
	$(CC) -m32 -Wl,-m,elf_i386 -T module_loader.ld -o loader.elf -ffreestanding -O2 -nostdlib $(filter-out $<,$^)
	grub-file --is-x86-multiboot loader.elf

FORCE: ;
jBoot/%: FORCE
	$(MAKE) -C $(@D) "$(notdir $@)" BOOT_FN=$(BOOT_FN) BOOT_EXT=$(BOOT_EXT)

.PHONY: clean
clean:
	rm -f ./bootstrap.o
	rm -f ./module_loader.o
	rm -f ./elf.o
	rm -f ./loader.elf
