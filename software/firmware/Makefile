
##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########------------------------------------------------------##########

MCU   = attiny84
F_CPU = 16000000UL
BAUD  = 9600UL
## Also try BAUD = 19200 or 38400 if you're feeling lucky.

##########------------------------------------------------------##########
##########                 Programmer Defaults                  ##########
##########          Set up once, then forget about it           ##########
##########        (Can override.  See bottom of file.)          ##########
##########------------------------------------------------------##########

PROGRAMMER_TYPE = usbtiny
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS =

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########     Won't need to change if they're in your PATH     ##########
##########------------------------------------------------------##########

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

# File paths
REFLOWOVEN_PATH = .
UART_LIB_PATH	= $(REFLOWOVEN_PATH)/avr-softuart
PATCH_PATH		= $(REFLOWOVEN_PATH)/lib_patches
BIN_DIR = bin
OBJ_DIR = obj

# Header paths
REFLOWOVEN_INC 	= $(REFLOWOVEN_PATH)/include
INCLUDES 		= $(REFLOWOVEN_INC:%=-I%)
UART_INCLUDES	= $(UART_LIB_PATH:%=-I%)

## The name of your project (without the .c)
TARGET = ReflowOven
## Or name it automatically after the enclosing directory
#TARGET = $(lastword $(subst /, ,$(CURDIR)))

UART_LIB 	= $(wildcard $(UART_LIB_PATH)/*.c)
SOURCES 	= $(wildcard $(REFLOWOVEN_PATH)/*.c)
UART_OBJ 	= $(UART_LIB:.c=.o)
OBJECTS 	= $(SOURCES:$(REFLOWOVEN_PATH)/%.c=$(OBJ_DIR)/%.o) $(UART_LIB:$(UART_LIB_PATH)/%.c=$(OBJ_DIR)/%.o)
HEADERS 	= $(SOURCES:.c=.h)

# Obtain library patches
PATCH = $(wildcard $(PATCH_PATH)/*.patch)
PATCHED = $(subst .patch,.patched,$(PATCH))

## Compilation options, type man avr-gcc if you're curious.
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I.
CPPFLAGS += $(INCLUDES)
CPPFLAGS += $(UART_INCLUDES)
CFLAGS = -Os -g -std=gnu99 -Wall
## Use short (8-bit) data types
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -Wl,-Map,$(BIN_DIR)/$(TARGET).map
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)

## Fuses for ATtiny84 w/16MHz CLK and 14CK startup time
LFUSE = 0xDF
HFUSE = 0xDF
EFUSE = 0xFF

FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

## Explicit pattern rules:
avr: $(OBJ_DIR)
avr: $(BIN_DIR)
avr: $(BIN_DIR)/$(TARGET).hex

$(PATCH_PATH)/%.patched:$(PATCH_PATH)/%.patch
	@if [ -e $@ ]; then \
		echo "Libraries Patched"; else \
		echo "Patching library"; \
		git submodule foreach git apply "../$<"; \
		cp $< $@; \
	fi;

$(OBJ_DIR):
	mkdir $@

$(BIN_DIR):
	mkdir $@

## To make .o files from .c files
$(OBJ_DIR)/%.o: $(REFLOWOVEN_PATH)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;
	@echo "Compiled "$<"\n"

## To make .o files from .c files
$(OBJ_DIR)/%.o: $(UART_LIB_PATH)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;
	@echo "Compiled "$<"\n"

$(BIN_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@
	@echo "Linking complete\n"

$(BIN_DIR)/%.hex: $(BIN_DIR)/$(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete\n"

#%.eeprom: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

#%.lst: %.elf
#	$(OBJDUMP) -S $< > $@

## These targets don’t have files named after them
.PHONY: all flash fuses clean size

all: avr

stage: $(PATCHED)

flash: $(BIN_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -U flash:w:$(BIN_DIR)/$(TARGET).hex

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(FUSE_STRING)

#show_fuse:
#	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -nv

clean:
	rm -f $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).hex $(BIN_DIR)/$(TARGET).map $(OBJECTS)
	rm $(PATCH_PATH)/*.patched
	# Remove library patches
	git submodule foreach git reset --hard

debug:
	@echo
	@echo "Source files:"   $(SOURCES)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo


# Optionally show how big the resulting program is
size:  $(BIN_DIR)/$(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(BIN_DIR)/$(TARGET).elf

