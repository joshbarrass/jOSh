KERNEL_DIR?=.
KERNEL_ARCH_DIR=$(KERNEL_DIR)/arch/$(TARGET_ARCH)

include $(KERNEL_ARCH_DIR)/make.config

KERNEL_PREFIX?=$(SYSROOT)/usr
KERNEL_INCLUDEDIR?=$(KERNEL_PREFIX)/include

KERNEL_OBJS=\
$(KERNEL_DIR)/kernel.o \
$(KERNEL_DIR)/tty.o \
$(KERNEL_DIR)/panic.o \
$(KERNEL_DIR)/mmap.o \
$(KERNEL_DIR)/bootstruct.o

KERNEL_INCLUDES=$(wildcard $(KERNEL_DIR)/include/**/*.h) $(wildcard $(KERNEL_DIR)/include/*.h)

KERNEL_CFLAGS?=-isystem=/usr/include -mcmodel=large -mno-sse -mno-sse2 -mno-mmx

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) -MD -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(KERNEL_CFLAGS)

$(KERNEL_DIR)/kernel.elf: $(KERNEL_ARCH_DIR)/linker.ld $(KERNEL_OBJS) $(KERNEL_ARCH_OBJS) $(LIBC_DIR)/libk.a
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib -static-libgcc $(CFLAGS) $(KERNEL_CFLAGS) $(filter-out $<,$^) -lgcc

.PHONY: kernel_clean
kernel_clean:
	rm -f '$(KERNEL_DIR)/kernel.elf'
	rm -f $(KERNEL_OBJS)
	rm -f $(KERNEL_OBJS:.o=.d)
	rm -f '$(KERNEL_DIR)/.headers_installed'

.PHONY: kernel_install-headers
kernel_install-headers: kernel_arch_install-headers
	mkdir -p '$(KERNEL_INCLUDEDIR)'
	cp -R --preserve=timestamps '$(KERNEL_DIR)/include/'* '$(KERNEL_INCLUDEDIR)/'

-include $(KERNEL_OBJS:.o=.d)
