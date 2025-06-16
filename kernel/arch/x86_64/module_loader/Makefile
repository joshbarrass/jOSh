OBJS=\
bootstrap.o \
module_loader.o \
elf.o \
addr_checker.o \
long_load.o

loader.elf: module_loader.ld $(OBJS)
	$(CC) -m32 -Wl,-m,elf_i386 -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(filter-out $<,$^)
	$(STRIP) --strip-unneeded '$@'
	grub-file --is-x86-multiboot '$@'

%.o: %.c
	$(CC) -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

%.o: %.nasm
	nasm -f elf32 -o '$@' '$<'

FORCE: ;
jBoot/%: FORCE
	$(MAKE) -C $(@D) "$(notdir $@)" BOOT_FN=$(BOOT_FN) BOOT_EXT=$(BOOT_EXT)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f ./loader.elf
