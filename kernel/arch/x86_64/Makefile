KERNEL_ARCH_ISO_DEPENDS=\
grubiso/boot/jOShload.elf

KERNEL_ARCH_OBJS=\
$(KERNEL_ARCH_DIR)/entry.o \
$(KERNEL_ARCH_DIR)/entry_asm.o \
$(KERNEL_ARCH_DIR)/ioports.o \
$(KERNEL_ARCH_DIR)/vga.o \
$(KERNEL_ARCH_DIR)/interrupts.o \
$(KERNEL_ARCH_DIR)/interrupt_handlers.o \
$(KERNEL_ARCH_DIR)/memory/pmm.o \
$(KERNEL_ARCH_DIR)/memory/vmm.o \
$(KERNEL_ARCH_DIR)/control_registers.o \
$(KERNEL_ARCH_DIR)/memory/recursive_pt.o \
$(KERNEL_ARCH_DIR)/memory/paging.o

ARCH_LOADER_DIR=$(KERNEL_ARCH_DIR)/module_loader
include $(ARCH_LOADER_DIR)/Makefile

$(KERNEL_ARCH_DIR)/%.o: $(KERNEL_ARCH_DIR)/%.c
	$(CC) -MD -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(KERNEL_CFLAGS)

$(KERNEL_ARCH_DIR)/%.o: $(KERNEL_ARCH_DIR)/%.nasm
	nasm -MD '$(KERNEL_ARCH_DIR)/$*.d' -f elf64 -o '$@' '$<'

grubiso/boot/jOShload.elf: $(ARCH_LOADER_DIR)/loader.elf
	mkdir -p grubiso/boot/
	cp '$(KERNEL_ARCH_DIR)/module_loader/loader.elf' grubiso/boot/jOShload.elf

.PHONY: kernel_arch_clean
kernel_arch_clean: arch_loader_clean
	rm -f $(KERNEL_ARCH_OBJS)
	rm -f $(KERNEL_ARCH_OBJS:.o=.d)

.PHONY: kernel_arch_install-headers
kernel_arch_install-headers:
	mkdir -p '$(KERNEL_INCLUDEDIR)'
	cp -R --preserve=timestamps '$(KERNEL_ARCH_DIR)/include/'* '$(KERNEL_INCLUDEDIR)/'

-include $(KERNEL_ARCH_OBJS:.o=.d)
