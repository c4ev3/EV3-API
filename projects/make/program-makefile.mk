## Copyright (c) 2015 Ahmad Fatoum
## Copyright (c) 2020 Jakub Vanek

##########################
# Project-level makefile #
##########################

# tool definitions
CROSS_COMPILE ?= C:/CSLite/bin/arm-none-linux-gnueabi-
CC       = $(CROSS_COMPILE)gcc
OBJCOPY  = $(CROSS_COMPILE)objcopy
STRIP    = $(CROSS_COMPILE)strip
MKDIR    = mkdir -p
RM       = rm -rf
EV3DUDER = ev3
BRICK_IP ?=

# define directories
OBJDIR = .objs
APIDIR ?= ../../..

# define output files
APPNAME = $(shell basename $(CURDIR))
ELF     = $(APPNAME).elf
RBF     = $(APPNAME).rbf
DBG     = $(ELF).dbg
REMDIR  = /home/root/lms2012/prjs/$(APPNAME)
REMFILE = $(REMDIR)/$(ELF)
REMRBF  = $(REMDIR)/$(RBF)
REMLOG  = $(patsubst %.elf,%.rtf,$(REMFILE))

# define file lists
SRCS     = $(wildcard *.c)
OBJS     = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
DEPFILES = $(patsubst %.c,$(OBJDIR)/%.d,$(SRCS))

# define flags
override CFLAGS += -std=c99
override CFLAGS += -fno-strict-aliasing -fwrapv
override CFLAGS += -Wall -Wextra -Wpointer-sign -Wno-unused-parameter
override CFLAGS += -D_GNU_SOURCE=1
override CXXFLAGS += -I $(APIDIR)/include -I $(APIDIR) -I $(APIDIR)/API

# optimization and debug flags
opt ?=
ifeq ($(opt),)
    RELFLAGS := -Os -g

else ifeq ($(opt),space)
    RELFLAGS := -Os -g

else ifeq ($(opt),fast)
    RELFLAGS := -O3 -g0

else ifeq ($(opt),debug)
    RELFLAGS := -O0 -ggdb3

else
    $(error unknown build type)
endif

# logging control
VERBOSE ?=
ifeq ($(VERBOSE)$(V),)
    Q := @@
else
    Q :=
endif

-include ../common-config.mk
-include project-config.mk

# default target
all: $(ELF) $(RBF)

# compile object files
$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d
	@echo " [CC]  $<"
	@$(MKDIR) $(@D)
	$(Q)$(CC) -MMD -MP $(RELFLAGS) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

$(DEPFILES):

-include $(DEPFILES)

# link final app
$(ELF) $(DBG): $(OBJS) $(APIDIR)/libev3api.a
	@echo " [LD]  $(@F)"
	$(Q)$(MKDIR) $(@D)
	$(Q)$(CC) $(RELFLAGS) $(LDFLAGS) $^ -o $@
	@echo " [DBG]  $(@F)"
	$(Q)$(OBJCOPY) --only-keep-debug "$@" "$@.dbg"
	$(Q)$(STRIP)   --strip-debug --strip-unneeded "$@"
	$(Q)$(OBJCOPY) --add-gnu-debuglink="$@.dbg" "$@"

# compile API
$(APIDIR)/libev3api.a:
	$(Q)$(MAKE) -C $(APIDIR) CROSS_COMPILE=$(CROSS_COMPILE) CFLAGS="$(CFLAGS) $(RELFLAGS)" libev3api.a

# create rbf
$(RBF): $(ELF)
	@echo " [RBF]  $(@F)"
	$(Q)$(EV3DUDER) mkrbf "$(REMFILE) >$(REMLOG) 2>&1" "$@"

# user-facing targets

help:
	@echo "Usage:"
	@echo "- make help     - show this message"
	@echo "- make build    - build this app"
	@echo "- make usbload  - upload this app via USB"
	@echo "- make netload  - upload this app via network"
	@echo "- make clean    - remove built files"
	@echo "- make cleanapi - remove built files of the EV3-API library"
	@echo "- make gdb      - open GDB for remote debugging"
	@echo "- make telnet   - open telnet session"
	@echo "Useful variables:"
	@echo "- make ... V=1"
	@echo "    -> show compilation commands"
	@echo "- make ... opt=space/debug/fast"
	@echo "    -> set default optimization target"
	@echo "- make ... BRICK_IP=192.168.1.42"
	@echo "    -> set brick IP address"
	@echo "- make ... CFLAGS='-flto' LDFLAGS='-flto'"
	@echo "    -> add compiler/linker flags"
	@echo "- make ... CROSS_COMPILE=arm-c4ev3-linux-uclibceabi-"
	@echo "    -> select the compiler used for build"

build: $(ELF)

usbload: $(ELF) $(RBF)
	@echo " [EV3]  mkdir $(REMDIR)"
	$(Q)$(EV3DUDER) mkdir          "$(REMDIR)" || true

	@echo " [EV3]  upload $(RBF) -> $(REMRBF)"
	$(Q)$(EV3DUDER) up    "$(RBF)" "$(REMRBF)"

	@echo " [EV3]  upload $(ELF) -> $(REMFILE)"
	$(Q)$(EV3DUDER) up    "$(ELF)" "$(REMFILE)"

ifeq ($(BRICK_IP),)
netload:
	@echo "please set BRICK_IP via configuration" && false
else
netload: $(ELF) $(RBF)
	@echo " [EV3]  mkdir $(REMDIR)"
	$(Q)$(EV3DUDER) --tcp=$(BRICK_IP) mkdir          "$(REMDIR)" || true

	@echo " [EV3]  upload $(RBF) -> $(REMRBF)"
	$(Q)$(EV3DUDER) --tcp=$(BRICK_IP) up    "$(RBF)" "$(REMRBF)"

	@echo " [EV3]  upload $(ELF) -> $(REMFILE)"
	$(Q)$(EV3DUDER) --tcp=$(BRICK_IP) up    "$(ELF)" "$(REMFILE)"
endif

# remove built files
clean:
	$(RM) $(OBJDIR) $(ELF) $(DBG) $(RBF)

cleanapi:
	$(Q)$(MAKE) -C $(APIDIR) clean

# run GDB for remote debugging
gdb: $(ELF) $(DBG)
	@echo " [GDB]  $<"
	$(Q)gdb-multiarch "$<"

# launch a telnet connection via network
ifeq ($(BRICK_IP),)
telnet:
	@echo "please set BRICK_IP via configuration" && false
else
telnet:
	telnet $(BRICK_IP) || true
endif

.PHONY: all help build usbload netload clean cleanapi gdb telnet
