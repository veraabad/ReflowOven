
##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########------------------------------------------------------##########

MCU   = attiny84
F_CPU = 8000000UL
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

## The name of your project (without the .c)
TARGET = ReflowOvenFirmware
## Or name it automatically after the enclosing directory
#TARGET = $(lastword $(subst /, ,$(CURDIR)))

SOURCES = ReflowOvenFirmware.c MAX31855.c softuart.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h)

## Compilation options, type man avr-gcc if you're curious.
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I.
CFLAGS = -Os -g -std=gnu99 -Wall
## Use short (8-bit) data types
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -Wl,-Map,$(TARGET).map
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)

## Fuses for ATtiny84
LFUSE = 0xE2
HFUSE = 0xDF
EFUSE = 0xFF

FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

## Explicit pattern rules:

## To make .o files from .c files
%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

#%.eeprom: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

#%.lst: %.elf
#	$(OBJDUMP) -S $< > $@

## These targets don’t have files named after them
.PHONY: all flash fuses clean size

all: $(TARGET).hex

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -U flash:w:$(TARGET).hex

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(FUSE_STRING)

#show_fuse:
#	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -nv

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).map $(TARGET).o

debug:
	@echo
	@echo "Source files:"   $(SOURCES)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo


# Optionally show how big the resulting program is
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

