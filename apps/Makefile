APPS_DIR?=.

APPS_PREFIX?=$(SYSROOT)/usr
APPS_INCLUDEDIR?=$(APPS_PREFIX)/include

APPS_DEPS=$(LIBC_DIR)/libk.a $(APPS_DIR)/kernel_iface.elf

APPS_ALL=$(APPS_DIR)/hello_world.elf

$(APPS_DIR)/libk.syms: $(LIBC_DIR)/libk.a
	nm -g --defined-only '$<' | awk '{print $$3}' | awk NF | sort > '$@'

$(APPS_DIR)/kernel.syms: $(KERNEL_DIR)/kernel.elf
	nm -g --defined-only '$<' | awk '{print $$3}' | awk NF | sort > '$@'

$(APPS_DIR)/retain_symbols_file: $(APPS_DIR)/libk.syms $(APPS_DIR)/kernel.syms
	comm -23 '$(APPS_DIR)/kernel.syms' '$(APPS_DIR)/libk.syms' > '$@'

$(APPS_DIR)/kernel_iface.elf: $(KERNEL_DIR)/kernel.elf $(APPS_DIR)/retain_symbols_file
	ld -r -R '$<' -o '$@' --retain-symbols-file '$(APPS_DIR)/retain_symbols_file'

$(APPS_DIR)/%.o: $(APPS_DIR)/%.c
	$(CC) -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(KERNEL_CFLAGS)

$(APPS_DIR)/%.elf: $(APPS_DIR)/%.ld $(APPS_DIR)/%.o $(APPS_DEPS)
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib -static-libgcc $(CFLAGS) $(KERNEL_CFLAGS) $(filter-out $<,$^) -lgcc

.PHONY: apps_clean
apps_clean:
	rm -f $(APPS_ALL)
	rm -f '$(APPS_DIR)/libk.syms' '$(APPS_DIR)/kernel.syms' '$(APPS_DIR)/retain_symbols_file' '$(APPS_DIR)/kernel_iface.elf'
	rm -f $(APPS_ALL:.elf=.o)
