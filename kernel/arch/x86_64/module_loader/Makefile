OBJS=\
bootstrap.o \
module_loader.o \
elf.o \
addr_checker.o \
long_load.o \
tty.o \
printf.o \
putchar.o \
puts.o \
errno.o \
strto.o \
isspace.o

loader.elf: module_loader.ld $(OBJS)
	$(CC) -m32 -Wl,-m,elf_i386 -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(filter-out $<,$^)
	$(STRIP) --strip-unneeded '$@'
	grub-file --is-x86-multiboot '$@'

tty.o: ../../../tty.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

%.o: ../../../../libc/stdio/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

%.o: ../../../../libc/stdlib/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

%.o: ../../../../libc/ctype/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

errno.o: ../../../../libc/errno/errno.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

%.o: %.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

%.o: %.nasm
	nasm -MD '$*.d' -f elf32 -o '$@' '$<'

FORCE: ;
jBoot/%: FORCE
	$(MAKE) -C $(@D) "$(notdir $@)" BOOT_FN=$(BOOT_FN) BOOT_EXT=$(BOOT_EXT)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)
	rm -f ./loader.elf

-include $(OBJS:.o=.d)
