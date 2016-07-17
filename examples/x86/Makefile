# Copyright (c) 2014 Bryan Batten <bjbatten@acm.org>
SHELL   :=/bin/bash

# ARCH is just one of the evaluation package's library directory names.
ARCH    ?=x86-generic
BINARY  ?=0
IPATH   ?=../..
SUFFIX ?=a

# Configure architecture
ifneq ($(findstring x86,$(ARCH)),)
    ifneq ($(findstring -64,$(ARCH)),)
        MARCH :=-march=$(shell echo $(ARCH)|cut -d',' -f1)
    else
        MARCH :=-mtune=$(shell echo $(ARCH)|cut -d- -f2)
        ifeq ($(MARCH),-mtune=pentium)
            MARCH :=$(MARCH) -m32
        endif
    endif

else
    $(error architecture "$(ARCH)" unsupported)
endif
MARCH :=$(strip $(MARCH))
ifneq ($(BINARY),0)
DEFINES :=-DUSE_BINARY_POINT
LPATH :=../../arch-binary
else
LPATH :=../../arch-decimal
endif
LIBPATH :=$(LPATH)/$(shell echo $(ARCH)|cut -d',' -f1)
# End configure architecture

CFLAGS +=-Wall -pedantic
CFLAGS += $(MARCH) $(DEFINES)
CFLAGS :=$(strip $(CFLAGS))
SRCS :=$(shell ls *.c)
EXAMPLES :=$(patsubst %.c,%,$(SRCS))

.PHONY: all clean cleantags tags

all: $(EXAMPLES)

cleantags:
	rm -f tags cscope.*

tags:
	@echo GEN tags
	@ctags -R
	@echo GEN cscope
	@cscope -bkq $(IPATH)/s16math.h $(shell find . -type f -name '*.[ch]*')

# keep just Makefile, README, *.c, *.h
clean: cleantags
	rm -f $(EXAMPLES) *.{[ab]*,[d-g]*,[i-z]*}

$(EXAMPLES): $(SRCS)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -o $@ $@.c -I$(IPATH) -lrt $(LIBPATH)/s16math.$(SUFFIX)
