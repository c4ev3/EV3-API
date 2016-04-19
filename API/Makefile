## Copyright (c) 2015 Ahmad Fatoum
override PREFIX = C:/CSLite/bin/arm-none-linux-gnueabi-
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

.DEFAULT: libev3api.a
libev3api.a: $(OBJS)
	$(AR) rcs $@ $^
	
%.o: %.c
	$(CC) -Os -isystem. -c $<

#ifeq ($(OS),Windows_NT)
#RM = del /Q
#endif

.PHONY: clean
clean:
	$(RM) *.o *.a *.d
