/*
	Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

	Permission is hereby granted, free of charge, to any person 
	obtaining a copy of this software and associated documentation 
	files (the "Software"), to deal in the Software without 
	restriction, including without limitation the rights to use, copy, 
	modify, merge, publish, distribute, sublicense, and/or sell copies 
	of the Software, and to permit persons to whom the Software is 
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be 
	included in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
	DEALINGS IN THE SOFTWARE.
	
	$Id$
	
	-----
	
	Modified by Alex from Inside Gadgets (http://www.insidegadgets.com)
	Last Modified: 22/08/2012

	Modified by Siddu Duddikunta <sduddikunta@gmail.com>
	Last Modified: 02/15/2013
*/

#include "mirf.h"
#include "nRF24L01.h"
#include "spi.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Defines for setting the MiRF registers for transmitting, receiving, powerdown
#define TX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) )
#define RX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) )
#define POWERDOWN mirf_config_register(CONFIG, mirf_CONFIG)

// Flag which denotes transmitting or receiving mode
volatile uint8_t PMODE;

// Initializes pins to communicate with the MiRF module
// Should be called in the early initializing phase at startup.
void mirf_init(void) {
	// Define CSN and CE as Output and set them to default
	DDRB |= ((1<<CSN)|(1<<CE));
	mirf_CE_lo;
	mirf_CSN_hi;
}

// Sets the important registers in the MiRF module and powers the module
// in receiving mode
void mirf_config(void) {
	// Set RF channel
	mirf_config_register(RF_CH, mirf_CH);

	// Set length of incoming payload 
	mirf_config_register(RX_PW_P0, mirf_PAYLOAD);
	
	// Set RADDR and TADDR
	mirf_write_register(RX_ADDR_P0, RADDR, 5);
	mirf_write_register(TX_ADDR, TADDR, 5);
	
	// Enable RX_ADDR_P0 address matching but disable auto ack, we want to avoid on-air ack collisions
	mirf_config_register(EN_RXADDR, 1<<ERX_P0);
	mirf_config_register(EN_AA, 0x00);

	PMODE = TXMODE; // Start in transmitting mode
	TX_POWERUP;     // Power up in transmitting mode
}

// Checks if data is available for reading
uint8_t mirf_data_ready(void) {
	mirf_CSN_lo; // Pull down chip select
	spi_transfer(R_REGISTER | (REGISTER_MASK & STATUS));
	uint8_t status = spi_transfer(NOP); // Read status register
	mirf_CSN_hi; // Pull up chip select
	return status & (1<<RX_DR);
}

// Checks if MAX_RT has been reached
uint8_t mirf_max_rt_reached(void) {
	mirf_CSN_lo; // Pull down chip select
	spi_transfer(R_REGISTER | (REGISTER_MASK & STATUS));
	uint8_t status = spi_transfer(NOP); // Read status register
	mirf_CSN_hi; // Pull up chip select
	return status & (1<<MAX_RT);
}

// Checks if payload was sent
uint8_t mirf_tx_ds(void) {
	mirf_CSN_lo; // Pull down chip select
	spi_transfer(R_REGISTER | (REGISTER_MASK & STATUS));
	uint8_t status = spi_transfer(NOP); // Read status register
	mirf_CSN_hi; // Pull up chip select
	return status & (1<<TX_DS);
}

// Reads mirf_PAYLOAD bytes into data array
void mirf_get_data(uint8_t *data) {
	mirf_CSN_lo; // Pull down chip select
	spi_transfer(R_RX_PAYLOAD); // Send cmd to read rx payload
	spi_read_data(data, mirf_PAYLOAD); // Read payload
	mirf_CSN_hi; // Pull up chip select
	mirf_config_register(STATUS,(1<<RX_DR)); // Reset status register
}

// Write one byte into the MiRF register
void mirf_config_register(uint8_t reg, uint8_t value) {
	mirf_CSN_lo;
	spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
	spi_transfer(value);
	mirf_CSN_hi;
}

// Reads an array of bytes from the MiRF registers.
void mirf_read_register(uint8_t reg, uint8_t *value, uint8_t len) {
	mirf_CSN_lo;
	spi_transfer(R_REGISTER | (REGISTER_MASK & reg));
	spi_read_data(value, len);
	mirf_CSN_hi;
}

// Writes an array of bytes into the MiRF register
void mirf_write_register(uint8_t reg, uint8_t *value, uint8_t len) {
	mirf_CSN_lo;
	spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
	spi_write_data(value, len);
	mirf_CSN_hi;
}

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void mirf_send(uint8_t *value, uint8_t len) {
	PMODE = TXMODE; // Set to transmitter mode
	TX_POWERUP; // Power up
	_delay_ms(5); // wait for transmitter to come up

	mirf_CSN_lo; // Pull down chip select
	spi_transfer(FLUSH_TX);  // Write cmd to flush tx fifo
	mirf_CSN_hi; // Pull up chip select

	mirf_CSN_lo;  // Pull down chip select
	spi_transfer(W_TX_PAYLOAD); // Write cmd to write payload
	spi_write_data(value, len); // Write payload
	mirf_CSN_hi; // Pull up chip select

	mirf_CE_hi; // Start transmission
	_delay_us(15);
	mirf_CE_lo;
}

void mirf_rx_mode()
{
	PMODE = RXMODE; // set to receive mode
	RX_POWERUP; // power up in RX mode
	_delay_ms(5); // wait for receiver to come up
	//spi_transfer(FLUSH_RX); // get rid of any stale data
	uint8_t buffer[1];
	while (mirf_data_ready()) mirf_get_data(buffer);
	mirf_CE_hi; // take CE high
}

void mirf_powerdown()
{
	mirf_CE_lo; // take ce low
	POWERDOWN; // turn off power
}
