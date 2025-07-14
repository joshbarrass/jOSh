LIBC_DIR?=.

LIBC_PREFIX?=$(SYSROOT)/usr
LIBC_INCLUDEDIR?=$(LIBC_PREFIX)/include

_LIBC_OBJ_NAMES=\
string/memmove \
stdio/printf

_LIBK_OBJS=$(addsuffix .libk.o, $(_LIBC_OBJ_NAMES))
_LIBC_OBJS=$(addsuffix .o, $(_LIBC_OBJ_NAMES))

LIBK_OBJS=$(addprefix $(LIBC_DIR)/, $(_LIBK_OBJS))
LIBC_OBJS=$(addprefix $(LIBC_DIR)/, $(_LIBC_OBJS))

LIBK_CFLAGS?=-D__is_libk -isystem=/usr/include
LIBC_CFLAGS?=-D__is_libc -isystem=/usr/include

$(LIBC_DIR)/%.o: $(LIBC_DIR)/%.c
	$(CC) -MD -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(LIBC_CFLAGS)

$(LIBC_DIR)/%.libk.o: $(LIBC_DIR)/%.c
	$(CC) -MD -c '$<' -o '$@' -ffreestanding $(CFLAGS) $(LIBK_CFLAGS)

$(LIBC_DIR)/libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

$(LIBC_DIR)/libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)

.PHONY: libc_clean
libc_clean:
	rm -f '$(LIBC_DIR)/libc.a' '$(LIBC_DIR)/libk.a'
	rm -f $(LIBC_OBJS)
	rm -f $(LIBC_OBJS:.o=.d)
	rm -f $(LIBK_OBJS)
	rm -f $(LIBK_OBJS:.o=.d)

.PHONY: libc_install-headers
libc_install-headers:
	mkdir -p '$(LIBC_INCLUDEDIR)'
	cp -R --preserve=timestamps '$(LIBC_DIR)/include/'* '$(LIBC_INCLUDEDIR)/'

-include $(LIBC_OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
