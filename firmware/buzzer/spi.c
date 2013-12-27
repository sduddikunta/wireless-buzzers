/*
	Copyright (c) 2013 Siddu Duddikunta <sduddikunta@gmail.com>

	All rights reserved. This software includes no warranty of any kind,
	express or implied.

	=======================================================================

	Functions for bit-bang SPI interface on the uSI pins of the ATtiny4313
*/

#include <avr/io.h>

uint8_t spi_shift(uint8_t shiftout)
{
	uint8_t result = 0x00;
	PORTB &= ~(1<<PB7); // clock low
	for (int i = 7; i >= 0; i--)
	{
		if ((shiftout & (1<<i)))
		{
			PORTB |= (1<<PB5);
		} else PORTB &= ~(1<<PB5);
		PORTB |= (1<<PB7); // clock high
		if ((PINB & (1<<PB6))) // input is high
		{
			result |= (1<<i); // set bit
		}
		PORTB &= ~(1<<PB7); // clock low
	}
	PORTB &= ~(1<<PB7); // clock low
	return result;
}

uint8_t spi_transfer(uint8_t data)
{
	return spi_shift(data);
}

void spi_write_data(uint8_t* dataout, uint8_t len)
{
	uint8_t i;
	for (i = 0; i < len; i++) {
		spi_transfer(dataout[i]);
	}
}

void spi_read_data(uint8_t* datain, uint8_t len)
{
	uint8_t i;
	for (i = 0; i < len; i++) {
		datain[i] = spi_transfer(0x00);
	}
}

void spi_setup(void)
{
	DDRB |= (1<<PB7); // PB7 is clock
	DDRB |= (1<<PB5); // PB5 is output data
	DDRB &= ~(1<<PB6); // PB6 is input data
	PORTB |= (1<<PB6); // pull up resistor on PB6
}
