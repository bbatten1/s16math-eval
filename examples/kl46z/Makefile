# vi:set ts=4: <-- vi tabstop
# Adapted from "Testshell" program downloaded from mbed.org.
# Copyright (c) 2014 Bryan Batten <bjbatten@acm.org>

# For more information,
# see http://mbed.org/handbook/Exporting-to-GCC-ARM-Embedded

# s16math include path and library location.
IPATH =../..
BINARY ?=0
ifneq ($(BINARY),0)
DEFINES :=-DUSE_BINARY_POINT
S16MATH_A=../../arch-binary/arm-cortex-m0+-l/s16math.a
else
S16MATH_A=../../arch-decimal/arm-cortex-m0+-l/s16math.a
endif

# Edit GCC_BIN to point to your local copy of the ARM embedded toolchain.
GCC_BIN =/usr/local/gcc-arm-none-eabi-4_8-2014q2/bin/
PROJECT = multest
OBJECTS = ./multest.o
SYS_OBJECTS = ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/startup_MKL46Z4.o \
			  ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/system_MKL46Z4.o \
			  ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
			  ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/mbed_overrides.o \
			  ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/board.o \
			  ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/retarget.o
INCLUDE_PATHS =-I. -I./mbed -I./mbed/TARGET_KL46Z \
			-I./mbed/TARGET_KL46Z/TARGET_Freescale \
			-I./mbed/TARGET_KL46Z/TARGET_Freescale/TARGET_KLXX \
			-I./mbed/TARGET_KL46Z/TARGET_Freescale/TARGET_KLXX/TARGET_KL46Z \
			-I./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM \
			-I$(IPATH)
LIBRARY_PATHS = -L./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM
#-L../../../arch-decimal/arm-cortex-m0plus-l
LIBRARIES = -lmbed
LINKER_SCRIPT = ./mbed/TARGET_KL46Z/TOOLCHAIN_GCC_ARM/MKL46Z4.ld

###############################################################################
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CXX     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy

CPU = -mcpu=cortex-m0plus -mthumb
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall \
		   -fno-exceptions -ffunction-sections -fdata-sections
CC_SYMBOLS = -DTARGET_KL46Z -DTARGET_M0P -DTARGET_Freescale -DTARGET_KLXX \
			 -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -D__CORTEX_M0 -DARM_MATH_CM0 \
			 -DMBED_BUILD_TIMESTAMP=1408683019.89 -D__MBED__=1 \
			 -DTARGET_FF_ARDUINO \
			 $(DEFINES)

LD_FLAGS = -mcpu=cortex-m0plus -mthumb -Wl,--gc-sections --specs=nano.specs \
		   -u _printf_float -u _scanf_float
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

all: $(PROJECT).bin

clean:
	rm -f $(PROJECT).bin $(PROJECT).elf $(OBJECTS)

.s.o:
	@echo "AS $<"
	@$(AS) $(CPU) -o $@ $<

.c.o:
	@echo "CC $<"
	@$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	@echo "CXX $<"
	@$(CXX) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 $(INCLUDE_PATHS) -o $@ $<


$(PROJECT).elf: $(OBJECTS) $(SYS_OBJECTS)
	@echo "LD $<"
	@$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) \
	-o $@ $^ $(S16MATH_A) $(LIBRARIES) $(LD_SYS_LIBS)

$(PROJECT).bin: $(PROJECT).elf
	@echo "OBJCOPY $<" $@
	@$(OBJCOPY) -O binary $< $@
