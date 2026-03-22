OBJS=\
bootstrap.o \
module_loader.o \
multiboot_header.o \
elf.o \
elf_paged.o \
addr_checker.o \
long_load.o \
tty.o \
printf.o \
putchar.o \
puts.o \
ops.o \
errno.o \
strto.o \
isspace.o \
bump_alloc.o \
paging.o \
bootstruct.o \
strcmp.o \
strlen.o \
ega_driver.o \
bitmap_console_driver.o \
bitmap_console_font.o \
null_console_driver.o \
bootstrap_display.o \
bootstrap_static_bump.o \
kmap.o

loader.elf: module_loader.ld $(OBJS)
	$(CC) -m32 -Wl,-m,elf_i386 -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(filter-out $<,$^) -lgcc
	$(STRIP) --strip-unneeded '$@'
	grub-file --is-x86-multiboot2 '$@'

tty.o: ../../../tty.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

bootstruct.o: ../../../bootstruct.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

%.o: ../../../../libc/stdio/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

%.o: ../../../../libc/stdlib/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

%.o: ../../../../libc/ctype/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

%.o: ../../../../libc/string/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

errno.o: ../../../../libc/errno/errno.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(CFLAGS)

ega_driver.o: ../../../drivers/ega/ega.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

bitmap_console_driver.o: ../../../drivers/bitmap_console/bitmap_console.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

bitmap_console_font.o: ../../../drivers/bitmap_console/default_font.psf
	cd '$(dir $(abspath $<))' && $(OBJCOPY) -O elf32-i386 -I binary '$(notdir $<)' '$(abspath $@)'

null_console_driver.o: ../../../drivers/null_console/null.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

bootstrap_%.o: ../../../bootstrap/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(CFLAGS)

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
