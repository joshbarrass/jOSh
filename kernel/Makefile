KERNEL_DIR?=.
ARCH_DIR=$(KERNEL_DIR)/arch/$(TARGET_ARCH)

include $(ARCH_DIR)/make.config

PREFIX?=$(SYSROOT)/usr
INCLUDEDIR?=$(PREFIX)/include

OBJS=\
$(KERNEL_DIR)/kernel.o \
$(KERNEL_DIR)/tty.o \
$(KERNEL_DIR)/panic.o

KERNEL_INCLUDES=$(wildcard $(KERNEL_DIR)/include/**/*.h) $(wildcard $(KERNEL_DIR)/include/*.h)

KERNEL_CFLAGS?=-isystem=/usr/include

KERNEL_HEADERS_INSTALLED=$(KERNEL_DIR)/.headers_installed

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c $(KERNEL_HEADERS_INSTALLED)
	$(CC) -MD -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(KERNEL_CFLAGS)

$(KERNEL_DIR)/kernel.elf: $(ARCH_DIR)/linker.ld $(OBJS)
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(KERNEL_CFLAGS) $(filter-out $<,$^)

$(KERNEL_DIR)/.headers_installed: $(KERNEL_INCLUDES)
	mkdir -p '$(INCLUDEDIR)'
	cp -R --preserve=timestamps '$(KERNEL_DIR)/include/'* '$(INCLUDEDIR)/'
	touch $(KERNEL_DIR)/.headers_installed

.PHONY: kernel_clean
kernel_clean:
	rm -f '$(KERNEL_DIR)/kernel.elf'
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)
	rm -f '$(KERNEL_DIR)/.headers_installed'

-include $(OBJS:.o=.d)
