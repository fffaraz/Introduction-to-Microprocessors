#
# a sample makefile
#
#
#CC = avr-gcc ${CFLAGS} -mmcu=atmega644p -Os
CC = avr-gcc ${CFLAGS} -mmcu=atmega1284p -Os
OBJCOPY = avr-objcopy
#BURN = avrdude -pm644p -c stk500 -P/dev/ttyS0 -e
#BURN = avrdude -pm1284p -c stk500 -P/dev/ttyUSB0 -e
BURN = avrdude -pm1284p -c stk500 -P/dev/ttyS0 -e

CFLAGS += -B/usr/avr/lib/avr5/ 
CFLAGS += -L/usr/avr/lib/
PROG=copyd-c

$(PROG).hex: $(PROG).elf
	$(OBJCOPY) -O ihex $< $@  

$(PROG).elf: $(PROG).c
	$(CC) $< -o $@

clean:
	rm -f *.o $(PROG).elf $(PROG).hex

burn: $(PROG).hex
	$(BURN)  -U flash:w:$(PROG).hex
