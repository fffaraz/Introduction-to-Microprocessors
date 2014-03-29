###############################################################################
# Makefile for the project nordic
###############################################################################

## General Flags
PROJECT = nordic
MCU = atmega644
TARGET = nordic.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99       -DF_CPU=8000000UL -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=nordic.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = tutorial2local.o spi1.o nrf24l01.o usart0.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) nordic.hex nordic.eep nordic.lss size

## Compile
tutorial2local.o: ../tutorial2local.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi1.o: ../spi1.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nrf24l01.o: ../nrf24l01.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart0.o: ../usart0.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) nordic.elf dep/* nordic.hex nordic.eep nordic.lss nordic.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

