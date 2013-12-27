/*
	Copyright (c) 2013 Siddu Duddikunta <sduddikunta@gmail.com>

	All rights reserved. This software includes no warranty of any kind,
	express or implied.

	=======================================================================

	Public header for SPI bit-bang functions defined in spi.c
*/

#ifndef _SPI_H
#define _SPI_H

uint8_t spi_shift(uint8_t shiftout);
uint8_t spi_transfer(uint8_t data);
void spi_write_data(uint8_t * dataout, uint8_t len);
void spi_read_data(uint8_t * datain, uint8_t len);
void spi_setup(void);

#endif // _SPI_H
