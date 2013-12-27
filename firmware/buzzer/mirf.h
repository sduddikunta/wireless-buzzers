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

	Modified by Siddu Duddikunta
	Last Modified: 02/15/2013
*/

#ifndef _MIRF_H_
#define _MIRF_H_

#include <avr/io.h>

// Mirf settings
#define RXMODE 1
#define TXMODE 0
#define mirf_CH			24
#define mirf_PAYLOAD		1
#define mirf_CONFIG		( (1<<MASK_TX_DS) | (1<<MASK_MAX_RT) | (1<<EN_CRC) | (0<<CRCO) )
#define RADDR				(uint8_t *)"chs02"
#define TADDR				(uint8_t *)"chs01"

// Pin definitions for chip select and chip enabled of the MiRF module
#define CE  PB1
#define CSN PB0

// Definitions for selecting and enabling MiRF module
#define mirf_CSN_hi	PORTB |=  (1<<CSN);
#define mirf_CSN_lo	PORTB &= ~(1<<CSN);
#define mirf_CE_hi	PORTB |=  (1<<CE);
#define mirf_CE_lo	PORTB &= ~(1<<CE);

// Public standard functions
extern void mirf_init(void);
extern void mirf_config(void);
extern void mirf_send(uint8_t * value, uint8_t len);
extern uint8_t mirf_data_ready(void);
extern uint8_t mirf_max_rt_reached(void);
extern void mirf_get_data(uint8_t * data);
extern void mirf_rx_mode();
extern void mirf_powerdown();
extern uint8_t mirf_tx_ds(void);

// Public extended functions
extern void mirf_config_register(uint8_t reg, uint8_t value);
extern void mirf_read_register(uint8_t reg, uint8_t * value, uint8_t len);
extern void mirf_write_register(uint8_t reg, uint8_t * value, uint8_t len);

#endif /* _MIRF_H_ */
