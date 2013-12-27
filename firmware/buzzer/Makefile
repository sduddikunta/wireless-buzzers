#	Copyright (c) 2013 Siddu Duddikunta <sduddikunta@gmail.com>
#
#	All rights reserved. This software includes no warranty of any kind,
#	express or implied.

SRC = $(wildcard *.c)
MCU = attiny4313
FLAGS = -DF_CPU=4000000 -DTHIS_BUZZER=$(BUZZER_ID) -Os -Wall -std=c99
OUTFILE = main

all: $(OUTFILE).hex

$(OUTFILE): $(SRC) $(wildcard *.h)
	avr-gcc -mmcu=$(MCU) $(FLAGS) -o $(OUTFILE) $(SRC)

$(OUTFILE).hex: $(OUTFILE)
	avr-objcopy -O ihex $(OUTFILE) $(OUTFILE).hex
	avr-size --mcu=$(MCU) -C $(OUTFILE)

clobber: clean

clean:
	rm -f $(OUTFILE) $(OUTFILE).hex

fuses:
	avrdude -c arduino -p attiny4313 -P /dev/ttyUSB0  -b 19200 -U lfuse:w:0xe2:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m

program: $(OUTFILE).hex
	avrdude -c arduino -p attiny4313 -P /dev/ttyUSB0  -b 19200 -U flash:w:$(OUTFILE).hex

bacon: clean fuses program
