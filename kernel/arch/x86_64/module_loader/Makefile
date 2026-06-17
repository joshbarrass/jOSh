ARCH_LOADER_DIR?=.
KERNEL_DIR?=../../..
LIBC_DIR?=../../../../libc
_ARCH_LOADER_OBJS=\
kernel_elf.o \
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
ARCH_LOADER_OBJS=$(addprefix $(ARCH_LOADER_DIR)/, $(_ARCH_LOADER_OBJS))

ARCH_LOADER_OBJCOPY_SYM_NAME=_binary_$(shell printf '%s' '$(KERNEL_DIR)/kernel.elf' | sed 's/[^[:alnum:]]/_/g')

ARCH_LOADER_CFLAGS?=$(CFLAGS) -isystem=/usr/include -msoft-float -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-mmx -DOBJCOPY_KERNEL_ELF_START='$(ARCH_LOADER_OBJCOPY_SYM_NAME)_start' -DOBJCOPY_KERNEL_ELF_SIZE='$(ARCH_LOADER_OBJCOPY_SYM_NAME)_size' -DOBJCOPY_KERNEL_ELF_END='$(ARCH_LOADER_OBJCOPY_SYM_NAME)_end'

$(ARCH_LOADER_DIR)/loader.elf: $(ARCH_LOADER_DIR)/module_loader.ld $(ARCH_LOADER_OBJS)
	$(CC) -m32 -Wl,-m,elf_i386 -T '$<' -o '$@' -ffreestanding -nostdlib $(ARCH_LOADER_CFLAGS) $(filter-out $<,$^) -lgcc
	$(STRIP) --strip-unneeded -K '$(ARCH_LOADER_OBJCOPY_SYM_NAME)_start' -K '$(ARCH_LOADER_OBJCOPY_SYM_NAME)_size' -K '$(ARCH_LOADER_OBJCOPY_SYM_NAME)_end' '$@'
	grub-file --is-x86-multiboot2 '$@'

$(ARCH_LOADER_DIR)/tty.o: $(KERNEL_DIR)/tty.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/bootstruct.o: $(KERNEL_DIR)/bootstruct.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/%.o: $(LIBC_DIR)/stdio/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/%.o: $(LIBC_DIR)/stdlib/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/%.o: $(LIBC_DIR)/ctype/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/%.o: $(LIBC_DIR)/string/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/errno.o: $(LIBC_DIR)/errno/errno.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding -D__is_libk $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/ega_driver.o: $(KERNEL_DIR)/drivers/ega/ega.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/bitmap_console_driver.o: $(KERNEL_DIR)/drivers/bitmap_console/bitmap_console.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/bitmap_console_font.o: $(KERNEL_DIR)/drivers/bitmap_console/default_font.psf
	cd '$(dir $(abspath $<))' && $(OBJCOPY) -O elf32-i386 -I binary '$(notdir $<)' '$(abspath $@)'

$(ARCH_LOADER_DIR)/null_console_driver.o: $(KERNEL_DIR)/drivers/null_console/null.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/bootstrap_%.o: $(KERNEL_DIR)/bootstrap/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/%.o: $(ARCH_LOADER_DIR)/%.c
	$(CC) -MD -m32 -c '$<' -o '$@' -ffreestanding $(ARCH_LOADER_CFLAGS)

$(ARCH_LOADER_DIR)/%.o: $(ARCH_LOADER_DIR)/%.nasm
	nasm -MD '$*.d' -f elf32 -o '$@' '$<'

$(ARCH_LOADER_DIR)/kernel_elf.o: $(KERNEL_DIR)/kernel.elf
	objcopy --input-target binary --output-target elf32-i386 --rename-section .data=.kernelelf,alloc,load,readonly,data,contents '$<' '$@'

.PHONY: arch_loader_clean
arch_loader_clean:
	rm -f $(ARCH_LOADER_OBJS)
	rm -f $(ARCH_LOADER_OBJS:.o=.d)
	rm -f ./loader.elf

-include $(ARCH_LOADER_OBJS:.o=.d)
