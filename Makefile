F_CPU = 1000000UL
BAUD = 9600UL
MCU = atmega328

LIBDIR = ./AVR-Programming-Library

CC = avr-gcc
OBJDUMP = avr-objdump
OBJCOPY = avr-objcopy 
AVRSIZE = avr-size
AVRDUDE = avrdude 

PROGRAMMER_ARGS = -b 19200 -P /COM4
PROGRAMMER_TYPE = avrisp

TARGET  = $(lastword $(subst /, , $(CURDIR)))
SOURCES = $(wildcard *.c $(LIBDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h)


## Compilation options, type man avr-gcc if you're curious.
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(LIBDIR)
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


%.o:%.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $@ -o $^;

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

all: $(TARGET).hex 


# Optionally show how big the resulting program is 
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

clean:
	del $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

##########------------------------------------------------------##########
##########              Programmer-specific details             ##########
##########           Flashing code to AVR using avrdude         ##########
##########------------------------------------------------------##########

flash: $(TARGET).hex 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -F -U flash:w:$<


debug:
	@echo 'TARGET  : ' $(TARGET)
	@echo 'SOURCES : ' $(SOURCES)
	@echo 'OBJECTS : ' $(OBJECTS)
	@echo 'HEADERS : ' $(HEADERS)