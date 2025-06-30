KERNEL_DIR?=.
ARCH_DIR=$(KERNEL_DIR)/arch/$(TARGET_ARCH)

include $(ARCH_DIR)/make.config

PREFIX?=$(SYSROOT)/usr
INCLUDEDIR?=$(PREFIX)/include

OBJS=\
$(KERNEL_DIR)/kernel.o \
$(KERNEL_DIR)/tty.o \
$(KERNEL_DIR)/panic.o

KERNEL_CFLAGS?=-isystem=/usr/include

$(KERNEL_DIR)/%.o: kernel_install_headers $(KERNEL_DIR)/%.c
	$(CC) -c '$(filter-out $<,$^)' -o '$@' -ffreestanding $(CFLAGS) $(KERNEL_CFLAGS)

$(KERNEL_DIR)/kernel.elf: $(ARCH_DIR)/linker.ld $(OBJS)
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(KERNEL_CFLAGS) $(filter-out $<,$^)

kernel_install_headers: $(KERNEL_DIR)/.headers_installed

$(KERNEL_DIR)/.headers_installed:
	mkdir -p '$(INCLUDEDIR)'
	cp -R --preserve=timestamps '$(KERNEL_DIR)/include/'* '$(INCLUDEDIR)/'
	touch $(KERNEL_DIR)/.headers_installed

.PHONY: kernel_clean
kernel_clean:
	rm -f '$(KERNEL_DIR)/kernel.elf'
	rm -f $(OBJS)
	rm -f '$(KERNEL_DIR)/.headers_installed'
