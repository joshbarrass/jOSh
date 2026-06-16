LOADER_DIR?=.
KERNEL_DIR?=../../..
LIBC_DIR?=../../../../libc
_LOADER_OBJS=\
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
LOADER_OBJS=$(addprefix $(LOADER_DIR)/, $(_LOADER_OBJS))

LOADER_CFLAGS?=$(CFLAGS) -isystem=/usr/include -msoft-float -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-mmx

$(LOADER_DIR)/loader.elf: $(LOADER_DIR)/module_loader.ld $(LOADER_OBJS)
	$(CC) -m32 -Wl,-m,elf_i386 -T '$<' -o '$@' -ffreestanding -nostdlib $(LOADER_CFLAGS) $(filter-out $<,$^) -lgcc
	$(STRIP) --strip-unneeded '$@'
	grub-file --is-x86-multiboot2 '$@'

$(LOADER_DIR)/tty.o: $(KERNEL_DIR)/tty.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/bootstruct.o: $(KERNEL_DIR)/bootstruct.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/%.o: $(LIBC_DIR)/stdio/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(LOADER_CFLAGS)

$(LOADER_DIR)/%.o: $(LIBC_DIR)/stdlib/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(LOADER_CFLAGS)

$(LOADER_DIR)/%.o: $(LIBC_DIR)/ctype/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(LOADER_CFLAGS)

$(LOADER_DIR)/%.o: $(LIBC_DIR)/string/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(LOADER_CFLAGS)

$(LOADER_DIR)/errno.o: $(LIBC_DIR)/errno/errno.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(LOADER_CFLAGS)

$(LOADER_DIR)/ega_driver.o: $(KERNEL_DIR)/drivers/ega/ega.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/bitmap_console_driver.o: $(KERNEL_DIR)/drivers/bitmap_console/bitmap_console.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/bitmap_console_font.o: $(KERNEL_DIR)/drivers/bitmap_console/default_font.psf
	cd '$(dir $(abspath $<))' && $(OBJCOPY) -O elf32-i386 -I binary '$(notdir $<)' '$(abspath $@)'

$(LOADER_DIR)/null_console_driver.o: $(KERNEL_DIR)/drivers/null_console/null.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/bootstrap_%.o: $(KERNEL_DIR)/bootstrap/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/%.o: $(LOADER_DIR)/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(LOADER_CFLAGS)

$(LOADER_DIR)/%.o: $(LOADER_DIR)/%.nasm
	nasm -MD '$*.d' -f elf32 -o '$@' '$<'

.PHONY: arch_loader_clean
arch_loader_clean:
	rm -f $(LOADER_OBJS)
	rm -f $(LOADER_OBJS:.o=.d)
	rm -f ./loader.elf

-include $(LOADER_OBJS:.o=.d)
