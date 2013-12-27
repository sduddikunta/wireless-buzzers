/* 
	Copyright (c) 2013 Siddu Duddikunta <sduddikunta@gmail.com>
	All rights reserved. This software includes no warranty,
	express or implied.

	=======================================================================

	Wireless buzzer system: master unit firmware
*/

#include <avr/io.h>
#include <util/delay.h>
#include <SPI.h>
#include "mirf.h"
#include "nRF24L01.h"

#define GREEN_ONE 0x11
#define RESET 0x01

volatile boolean resetP = false;
int countdown = 500;
long milli = -1;

// 7-segment digits from 0 to 9
byte digit[] = {0b01111110, 0b00110000, 0b01101101, 0b01111001, 0b00110011, 0b01011011, 0b01011111, 0b01110000, 0b01111111, 0b01110011};

#include "mirf.c"

void reset() {
  resetP = true;
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mirf_init();
  _delay_ms(50);
  mirf_config();
  pinMode(2, INPUT);
  pinMode(A3, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(4, OUTPUT);
  PORTC = 0;
  digitalWrite(2, HIGH);
  attachInterrupt(0, reset, FALLING);
  digitalWrite(4, HIGH); // chip select high
  delay(10); // wait a bit
  digitalWrite(4, LOW); // chip select low
  shiftOut(A4, A5, MSBFIRST, 0x09); // decode mode register
  shiftOut(A4, A5, MSBFIRST, 0x00); // no decode
  digitalWrite(4, HIGH); // chip select high
  digitalWrite(4, LOW); // chip select low
  shiftOut(A4, A5, MSBFIRST, 0x0A); // intensity register
  shiftOut(A4, A5, MSBFIRST, 0x0A); // intensity 3/16
  digitalWrite(4, HIGH); // chip select high
  digitalWrite(4, LOW); // chip select low
  shiftOut(A4, A5, MSBFIRST, 0x0B); // scan limit register
  shiftOut(A4, A5, MSBFIRST, 0x07); // scan all 8 digits
  digitalWrite(4, HIGH); // chip select high
  digitalWrite(4, LOW); // chip select low
  shiftOut(A4, A5, MSBFIRST, 0x0C); // shutdown register
  shiftOut(A4, A5, MSBFIRST, 0x01); // not in shutdown
  digitalWrite(4, HIGH); // chip select high
  delay(2000);
  digitalWrite(4, LOW); // chip select low
  shiftOut(A4, A5, MSBFIRST, 4); // digit
  shiftOut(A4, A5, MSBFIRST, 128); // DP
  digitalWrite(4, HIGH); // chip select high
  digitalWrite(4, LOW); // chip select low
  shiftOut(A4, A5, MSBFIRST, 8); // digit
  shiftOut(A4, A5, MSBFIRST, 128); // DP
  digitalWrite(4, HIGH); // chip select high
}
void loop() {
  uint8_t buffer[1] = {0};
  uint8_t buffersize = 1;
  int channelcount = 0;
  while(1) {
    mirf_CSN_lo; // Pull down chip select
    SPI.transfer(FLUSH_RX);  // Write cmd to flush tx fifo
    mirf_CSN_hi; // Pull up chip select
    while (mirf_data_ready()) mirf_get_data(buffer);
    buffer[0] = 0;
    mirf_CE_hi; // Start listening
    while (!mirf_data_ready()) {
      if (resetP) {
        mirf_CE_lo;
            buffer[0] = RESET;
            for (int i = 0; i < 8; i++){
              mirf_send(buffer, 1); // send the reset value
              _delay_ms(10); // wait for it to send
            }
            PMODE = RXMODE; // Start in receiving mode
            RX_POWERUP;     // Power up in receiving mode
            buffer[0] = 0;  // reset our buffer
            _delay_ms(5);
            resetP = false;
            mirf_CE_hi;
      }
      _delay_us(50);
       Serial.print(".");
    }
    mirf_get_data(buffer);
    if (buffer[0] < 0x11 || buffer[0] > 0x26) {
      return;
    }
    Serial.println();
    Serial.println(buffer[0], DEC);
    mirf_CE_lo; // get out of receive
    _delay_ms(50); // wait a bit for the receiver to get ready
    for (int i = 0; i < 8; i++){
    mirf_send(buffer, 1); // send back what we got
    _delay_ms(10);
    }
    if (buffer[0] > 0x20 && buffer[0] < 0x30) { // red team
      drive(120, 284);
      delay(15);
      drive(60, 568);
      delay(15);
      drive(100, 284);
    } else if (buffer[0] > 0x10 && buffer[0] < 0x20) { // green team
      drive(60, 568);
      delay(15);
      drive(100, 284);
      delay(15);
      drive(60, 568);
    }
    resetP = false;
    count(buffer[0]);
    _delay_ms(50); // wait a bit
    buffer[0] = RESET;
    for (int i = 0; i < 8; i++){
    mirf_send(buffer, 1); // send the reset value
    _delay_ms(10); // wait for it to send*/
    }
    PMODE = RXMODE; // Start in receiving mode
    RX_POWERUP;     // Power up in receiving mode
    buffer[0] = 0;  // reset our buffer
    _delay_ms(5);
    resetP = false;
  }
}

void drive(int duration, int time)
{
  for (int i = 0; i < duration; i++) {
    PORTC = B00000010;
    delayMicroseconds(time);
    PORTC = B00001000;
    delayMicroseconds(time);
  }
  PORTC = 0;
}
  
void count(byte buzzer) {
    countdown = 500;
    milli = -1;
    for (int i = 1; i <= 8; i++) {
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, i); // digit
      shiftOut(A4, A5, MSBFIRST, 0x00); // all off
      digitalWrite(4, HIGH); // chip select high
    }
    if (buzzer > 0x10 && buzzer < 0x20) {
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 4); // digit
      if (buzzer == 0x11) shiftOut(A4, A5, MSBFIRST, digit[1]);
      if (buzzer == 0x12) shiftOut(A4, A5, MSBFIRST, digit[2]);
      if (buzzer == 0x13) shiftOut(A4, A5, MSBFIRST, digit[3]);
      if (buzzer == 0x14) shiftOut(A4, A5, MSBFIRST, digit[4]);
      if (buzzer == 0x15) shiftOut(A4, A5, MSBFIRST, digit[5]);
      if (buzzer == 0x16) shiftOut(A4, A5, MSBFIRST, digit[6]);
      digitalWrite(4, HIGH); // chip select high
    }
    if (buzzer > 0x20 && buzzer < 0x30) {
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 8); // digit
      if (buzzer == 0x21) shiftOut(A4, A5, MSBFIRST, digit[1]);
      if (buzzer == 0x22) shiftOut(A4, A5, MSBFIRST, digit[2]);
      if (buzzer == 0x23) shiftOut(A4, A5, MSBFIRST, digit[3]);
      if (buzzer == 0x24) shiftOut(A4, A5, MSBFIRST, digit[4]);
      if (buzzer == 0x25) shiftOut(A4, A5, MSBFIRST, digit[5]);
      if (buzzer == 0x26) shiftOut(A4, A5, MSBFIRST, digit[6]);
      digitalWrite(4, HIGH); // chip select high
    }
    resetP = false;
    while (!resetP) {
      if (millis() - milli > 9)
      countdown--;
      else continue;
      milli = millis();
      if (countdown < 0) {
        drive(80, 1000);
        delay(10);
        drive(80, 1000);
        delay(10);
        drive(80, 1000);
        digitalWrite(A1, LOW);
        digitalWrite(A3, LOW);
        while (!resetP) {}
      }
      if (buzzer > 0x20 && buzzer < 0x30) {
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 4);
      shiftOut(A4, A5, MSBFIRST, digit[countdown % 10]);
      digitalWrite(4, HIGH); // chip select high
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 3);
      shiftOut(A4, A5, MSBFIRST, digit[(countdown / 10) % 10]);
      digitalWrite(4, HIGH);
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 2);
      shiftOut(A4, A5, MSBFIRST, digit[countdown / 100] | 0x80);
      digitalWrite(4, HIGH);
      } else {
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 8);
      shiftOut(A4, A5, MSBFIRST, digit[countdown % 10]);
      digitalWrite(4, HIGH); // chip select high
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 7);
      shiftOut(A4, A5, MSBFIRST, digit[(countdown / 10) % 10]);
      digitalWrite(4, HIGH);
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, 6);
      shiftOut(A4, A5, MSBFIRST, digit[countdown / 100] | 0x80);
      digitalWrite(4, HIGH);
      }
    }
    for (int i = 1; i <= 8; i++) {
      digitalWrite(4, LOW); // chip select low
      shiftOut(A4, A5, MSBFIRST, i); // digit
      shiftOut(A4, A5, MSBFIRST, 0x00); // all off
      digitalWrite(4, HIGH); // chip select high
    }
    digitalWrite(4, LOW); // chip select low
    shiftOut(A4, A5, MSBFIRST, 4); // digit
    shiftOut(A4, A5, MSBFIRST, 128); // DP
    digitalWrite(4, HIGH); // chip select high
    digitalWrite(4, LOW); // chip select low
    shiftOut(A4, A5, MSBFIRST, 8); // digit
    shiftOut(A4, A5, MSBFIRST, 128); // DP
    digitalWrite(4, HIGH); // chip select high
}
