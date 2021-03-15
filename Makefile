## Copyright (c) 2015 Ahmad Fatoum

# define commands
CROSS_COMPILE ?= arm-none-linux-gnueabi-
PREFIX ?= $(CROSS_COMPILE)
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
SED = sed
MKDIR = IF NOT Exist $(subst /,\,$(@D)) (mkdir $(subst /,\,$(@D)))
# IF no exist myDirName (mkdir myDirName)
INSTALL = cp
RM = rm -rf

# define directories
# - for build
OBJDIR := .objs
# - for install
DESTDIR    ?= $(CURDIR)/installed
INCLUDEDIR ?= $(DESTDIR)/include
LIBDIR     ?= $(DESTDIR)/lib
APIDIR ?= C:\GitHub\EV3-API

# define files
SRCS     = $(wildcard API/*.c API/**/*.c contrib/**/*.c)
OBJS     = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
DEPFILES = $(patsubst %.c,$(OBJDIR)/%.d,$(SRCS))

# define flags
# ARM926EJ-S
# -mcpu=arm926ej-s
override CFLAGS += -mtune=arm926ej-s -mcpu=arm926ej-s
override CFLAGS += -std=c99
override CFLAGS += -fno-strict-aliasing -fwrapv
override CFLAGS += -Wall -Wextra -Wpointer-sign -Wno-unused-parameter
override CFLAGS += -D_GNU_SOURCE=1
override CFLAGS += -fdata-sections -ffunction-sections
#override CFLAGS += -Wl, -gc-sections
# logging control
ifeq ($(VERBOSE)$(V),)
  Q = @@
else
  Q = 
endif
	
# -include api-config.mk

# library building

all: libev3api.a

libev3api.a: $(OBJS)
	@echo " [AR]  $@"
	$(Q)$(AR) rcs $@ $^
	
$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d
	@echo " [CC]  $<"
#	@$(MKDIR) $(subst /,\,$(@D))
	@$(MKDIR)
	$(Q)$(CC) -isystem include -MMD -MP -O3 -g0 $(CFLAGS) -isystem. -I API -c $< -o $@
# $(Q)$(CC) -isystem include -MMD -MP -Os $(CFLAGS) -isystem. -I API -c $< -o $@	
$(DEPFILES):
	@$(MKDIR) 	

-include $(DEPFILES)

# pkgconfig processing & installation

install: libev3api.a API/libev3api.pc.in
	@# this cannot be a target because it depends on the variable value
	@echo " [SED]  libev3api.pc"
	$(Q)$(SED) -e "s+@PREFIX@+$(DESTDIR)+" \
	           -e "s+@INCDIR@+$(INCLUDEDIR)+" \
	           -e "s+@LIBDIR@+$(LIBDIR)+" \
	           API/libev3api.pc.in > libev3api.pc

	@echo " [INSTALL]  libev3api.a"
	$(Q)$(MKDIR) $(LIBDIR)/pkgconfig $(INCLUDEDIR)/ev3api
	$(Q)$(INSTALL) libev3api.a  $(LIBDIR)/

	@echo " [INSTALL]  libev3api.pc"
	$(Q)$(INSTALL) libev3api.pc $(LIBDIR)/pkgconfig/

	@echo " [INSTALL]  API/*.h"
	$(Q)$(INSTALL) API/*.h      $(INCLUDEDIR)/ev3api/

uninstall:
	$(RM) $(LIBDIR)/libev3api.a            \
	      $(LIBDIR)/pkgconfig/libev3api.pc \
	      $(INCLUDEDIR)/ev3api


# sanity check helper

example:
	echo 'int main(void) { return EV3IsInitialized() == 1; }' | $(CC) -xc $(CFLAGS) - -L. -lev3api -IAPI/include -oexample -include ev3.h

# cleanup

clean:
	$(RM) $(OBJDIR) *.a *.pc example

.PHONY: all clean install uninstall
