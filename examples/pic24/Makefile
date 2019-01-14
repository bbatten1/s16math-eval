# vi:set ts=4: <-- vi tabstop
# Copyright (c) 2018 Bryan Batten <bjbatten@acm.org>

SHELL   :=/bin/bash
MCU ?=24FJ128GA204

# s16math include path
IPATH =../..
BINARY ?=0
ifneq ($(BINARY),0)
DEFINES :=-DUSE_BINARY_POINT
S16MATH_A=../../arch-binary/pic24FJ128GA204/s16math.a
else
S16MATH_A=../../arch-decimal/pic24FJ128GA204/s16math.a
endif

# GDB=-g on command line for debug info
GDB     ?=
ifneq ($(GDB),)
    DEFINES +=-DDEBUG
endif

AS :=xc16-as
AR :=xc16-ar
CC :=xc16-gcc
LD :=xc16-ld
# Override only if at GNU ARFLAGS default setting
ifeq ($(ARFLAGS),rv)
ARFLAGS :=vsru
endif
# set up Binary File Descriptor handler
OBJCOPY :=xc16-bin2hex
OPT		:=-mcpu=$(MCU) -mcci -O1
CFLAGS  +=-std=gnu99 -Wall -pedantic
CFLAGS  += $(OPT) $(GDB) $(DEFINES)
CFLAGS  :=$(strip $(CFLAGS))

TESTSRCS=testmul.c
TESTS=$(patsubst %.c,%,$(TESTSRCS))
HEX=$(patsubst %,%.hex,$(TESTS))
SYSSRCS=$(shell find ./mcc_generated_files -maxdepth 2 -name "*.c")
SYSHDRS=$(shell find ./mcc_generated_files -maxdepth 2 -name "*.h")
SYSOBJS :=$(patsubst %.c,%.o,$(SYSSRCS))
SYSINCS =-I. -I./mcc_generated_files -I./mcc_generated_files/drivers -I$(IPATH)
GLD		=-T./gld/p$(MCU).gld

.PHONY: all tags cleantags cleantests cleanobjs clean obj test
all: test

cleantags:
	rm -f tags cscope.*

cleanobjs:
	@rm -f $(SYSOBJS)

cleantests:
	@rm -f $(TESTS) $(HEX)

tags:
	@echo GEN tags
	@ctags -R $(IPATH)/s16math.h
	@echo GEN cscope
	@cscope -bkq $(IPATH)/s16math.h $(shell find . -type f -name '*.[ch]*')

clean: cleantags cleantests cleanobjs

$(SYSOBJS): %.o: %.c $(SYSHDRS)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $(SYSINCS) $(GLD) -o $@ $<

obj: $(SYSOBJS)

# works for (x86) cc, arm-linux-gnueabi-gcc
$(TESTS): %: %.c $(SYSHDRS) $(SYSOBJS) $(IPATH)/s16math.h $(S16MATH_A)
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(SYSINCS) $(GLD) -o $@ $< $(SYSOBJS) $(S16MATH_A)
	@$(OBJCOPY) $@

test: $(TESTS)
