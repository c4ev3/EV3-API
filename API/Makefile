## Copyright (c) 2015 Ahmad Fatoum
CROSS_COMPILE ?= C:/CSLite/bin/arm-none-linux-gnueabi-
PREFIX ?= $(CROSS_COMPILE)
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
CFLAGS += -fno-strict-aliasing

.DEFAULT: libev3api.a
libev3api.a: $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -Os $(CFLAGS) -isystem. -c $<

example:
	echo 'int main(void) { return EV3IsInitialized() == 1; }' | $(CC) -xc $(CFLAGS) - -L. -lev3api -I. -oexample -include ev3.h

#ifeq ($(OS),Windows_NT)
#RM = del /Q
#endif

.PHONY: clean
clean:
	$(RM) *.o *.a *.d example
