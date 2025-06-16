ARCH_DIR=arch/$(TARGET_ARCH)
override CFLAGS+=-isystem=/usr/include

include $(ARCH_DIR)/make.config

PREFIX?=$(SYSROOT)/usr
INCLUDEDIR?=$(PREFIX)/include

OBJS=\
kernel.o \
tty.o \
panic.o

FORCE: ;

%.o: %.c
	$(CC) -c '$<' -o '$@' -ffreestanding $(CFLAGS)

kernel.elf: $(ARCH_DIR)/linker.ld $(OBJS)
	$(CC) -T '$<' -o '$@' -ffreestanding -nostdlib $(CFLAGS) $(filter-out $<,$^)

.PHONY: test
test: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -gdb tcp::9000

.PHONY: debug
debug: jOSh.iso
	qemu-system-$(TARGET_ARCH) -cdrom '$<' -boot order=d -s -S

.PHONY: clean
clean: arch_clean
	rm -f $(OBJS)
	rm -f ./kernel.elf
	rm -f jOSh.iso
	rm -f .headers_installed

install-headers: .headers_installed

.headers_installed:
	mkdir -p $(INCLUDEDIR)
	cp -R --preserve=timestamps include/* $(INCLUDEDIR)/
	touch .headers_installed
