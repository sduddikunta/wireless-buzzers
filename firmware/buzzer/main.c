/*
	Copyright (c) 2013 Siddu Duddikunta <sduddikunta@gmail.com>
	
	All rights reserved. This software includes no warranty of any kind,
	express or implied.

	=======================================================================	

	Main wireless buzzer firmware
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include "mirf.h"
#include "spi.h"
#include "nRF24L01.h"

#define RESET 0x01

volatile uint8_t sleep = 0;
volatile uint8_t countdown = 30;

void process_recv()
{
	if (mirf_data_ready())
	{
		uint8_t buffer[1];
		mirf_get_data(buffer);
		if (buffer[0] == THIS_BUZZER)
		{
			PORTD |= (1<<PD4); // light led
		}
		if (buffer[0] == RESET)
		{
			sleep = 1;
		}
	}
}

int main(void)
{
	spi_setup();
	DDRD |= (1<<PD4); // led is output
	PORTD |= (1<<PD2); // pull up resistor on INT0
	MCUCR |= (1<<ISC01); // interrupt 0 is falling edge
	sei();
	PORTD |= (1<<PD4); // flash led
	mirf_init();
	mirf_config();
	_delay_ms(250);
	PORTD &= ~(1<<PD4);
	TCCR1A = 0x00; // setting up our timer
	TCCR1B = (1<<WGM12) | (1<<CS12); // ctc mode, prescale 256
	OCR1A = 15625; // this equals 1hz
	while(1) {
		EIFR = (1<<INT0); // clear the INT0 flag
		// note above: EIFR was renamed to GIFR, but the headers aren't updated
		GIMSK |= (1<<INT0); // enable INT0 so we can wake up
		mirf_powerdown();
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_enable(); 
		sei();
		sleep_cpu();
		sleep_disable();
		mirf_rx_mode();
		sleep = 0;
		countdown = 30;
		TIMSK |= (1<<OCIE1A); // turn on timer
		while (!sleep)
		{
			process_recv();
		}
		TIMSK &= ~(1<<OCIE1A); // turn off timer
		PORTD &= ~(1<<PD4);
	}
}

ISR (TIMER1_COMPA_vect)
{
	countdown--;
	if (countdown == 0)
		sleep = 1;
}

ISR (INT0_vect)
{
	GIMSK &= ~(1<<INT0); // turn off interrupt
	sleep_disable();
	uint8_t buffer[1] = {THIS_BUZZER};
	uint8_t buffersize = 1;
	for (int i = 0; i < 8; i++)
	{
		mirf_send(buffer, buffersize);
		while (!mirf_tx_ds()) {}
		mirf_config_register(STATUS, 1<<TX_DS);
	}
}
