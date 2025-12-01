APPS_DIR?=.

APPS_PREFIX?=$(SYSROOT)/usr
APPS_INCLUDEDIR?=$(APPS_PREFIX)/include

APPS_DEPS=$(LIBC_DIR)/libk.a $(APPS_DIR)/kernel_iface.elf

$(APPS_DIR)/kernel_iface.elf: $(KERNEL_DIR)/kernel.elf
	ld -r -R '$<' -o '$@'

$(APPS_DIR)/%.o: $(APPS_DIR)/%.c
	$(CC) -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(KERNEL_CFLAGS)

$(APPS_DIR)/hello_world.elf: $(APPS_DIR)/hello_world.ld $(APPS_DIR)/hello_world.o $(APPS_DEPS)
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib -static-libgcc $(CFLAGS) $(KERNEL_CFLAGS) $(filter-out $<,$^) -lgcc
