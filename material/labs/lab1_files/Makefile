#
# a sample makefile to compile a single program test.c
#
#
#CC = avr-gcc -mmcu=atmega644p -D F_CPU=1000000 -Os
CC = avr-gcc -mmcu=atmega1284p -D F_CPU=1000000 -Os
OBJCOPY = avr-objcopy
BURN = avrdude -pm1284p -c stk500 -P/dev/ttyS0 -e

PROG=test

$(PROG).hex: $(PROG).elf
	$(OBJCOPY) -O ihex $< $@  

$(PROG).elf: $(PROG).c
	$(CC) $< -o $@

clean:
	rm -f *.o $(PROG).elf $(PROG).hex

burn: $(PROG).hex
	$(BURN)  -U flash:w:$(PROG).hex
