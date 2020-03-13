## Copyright (c) 2015 Ahmad Fatoum

# define commands
CROSS_COMPILE ?= C:/CSLite/bin/arm-none-linux-gnueabi-
PREFIX ?= $(CROSS_COMPILE)
DESTDIR ?= $(CURDIR)
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
SED = sed
MKDIR = mkdir -p
INSTALL = cp
RM = rm -rf

# define files
SRCS = $(wildcard API/*.c contrib/**/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

override CFLAGS += -std=c99
override CFLAGS += -fno-strict-aliasing -fwrapv
override CFLAGS += -Wall -Wextra -Wpointer-sign -Wno-unused-parameter
override CFLAGS += -D_GNU_SOURCE=1

# library building

.DEFAULT: libev3api.a

libev3api.a: $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -Os $(CFLAGS) -isystem. -c $< -o $@ 

# pkgconfig processing & installation

libev3api.pc: libev3api.pc.in
	$(SED) -e "s+@PREFIX@+$(DESTDIR)+" $< > $@

install: libev3api.a libev3api.pc
	$(MKDIR) $(DESTDIR)/lib $(DESTDIR)/share/pkgconfig $(DESTDIR)/include/ev3api
	$(INSTALL) libev3api.a  $(DESTDIR)/lib/
	$(INSTALL) *.h          $(DESTDIR)/include/ev3api/
	$(INSTALL) libev3api.pc $(DESTDIR)/share/pkgconfig/

uninstall:
	$(RM) $(DESTDIR)/lib/libev3api.pc  $(DESTDIR)/share/pkgconfig/libev3api.pc $(DESTDIR)/include/ev3api

# sanity check helper

example:
	echo 'int main(void) { return EV3IsInitialized() == 1; }' | $(CC) -xc $(CFLAGS) - -L. -lev3api -I. -oexample -include ev3.h

# cleanup

clean:
	$(RM) API/*.o contrib/**/*.o *.a *.d example

.PHONY: clean install uninstall
