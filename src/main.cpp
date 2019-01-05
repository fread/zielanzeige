#include <Arduino.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>

#include "font.h"

const int width = 120;
const int lines = 7;

const int line_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

// Shift register needs to use raw AVR
// B1 is clock
// B2 is data
void shiftout(uint8_t bit)
{
	PORTB = bit << 2;
	PORTB |= 0x02;
	PORTB = 0;
}

void setup()
{
	for (uint8_t i = 2; i <= 10; i++) {
		pinMode(i, OUTPUT);
	}
	pinMode(13, OUTPUT);
}

char *message = "Hello world!        ";

void loop()
{
	for (uint8_t y = 0; y < lines; y++) {
		uint8_t letter = 0;
		uint8_t lettercol = 0;
		for (uint8_t x = 0; x < width; x++) {
			if (lettercol == 5) {
				lettercol = 0;
				letter++;
				shiftout(0);
			} else {
				uint8_t col = pgm_read_byte_near(&font_5x7_col[message[letter]][lettercol]);
				uint8_t bit = (col & (1 << y)) ? 1 : 0;
				shiftout(bit);
				lettercol++;
			}
		}

		digitalWrite(13, 1);
		digitalWrite(line_pins[y], 1);
		delayMicroseconds(200);
		digitalWrite(13, 0);
		digitalWrite(line_pins[y], 0);
	}
}
