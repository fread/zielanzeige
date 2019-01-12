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

uint8_t buffer[120];
String message = "N\x0f""chster Halt: Kronenplatz";
String newMessage;
int ticks = 0;
int runde = 120;

void write_letter_at(int startcol, uint8_t letter) {
	for (int8_t x = 0; x < 5; x++) {
		if (startcol + x >= 0 && startcol + x < width) {
			buffer[startcol + x] = pgm_read_byte_near(&font_5x7_col[letter][x]);
		}
	}
}

void setup()
{
	for (uint8_t i = 2; i <= 10; i++) {
		pinMode(i, OUTPUT);
	}
	pinMode(13, OUTPUT);

	write_letter_at(0,  'H');
	write_letter_at(6,  'i');
	write_letter_at(12, '!');

	Serial.begin(9600);
}

void receive_serial() {
	if (Serial.available() > 0) {
		char receivedChar = Serial.read();
		if (receivedChar == '\n') {
			message = newMessage;
			newMessage = "";
			ticks = 0;
			runde = 120;
		}
		else if (receivedChar != '\r') {
			newMessage += receivedChar;
		}
	}
}

void letters(int round) {
	for (int i = 0; i < width; i++) {
		buffer[i] = 0;
	}

	int pos = 0;
	for (unsigned c = 0; c < message.length(); c++) {
		write_letter_at(round + pos, message[c]);
		pos += 6;
	}
}

void loop()
{
	for (uint8_t y = 0; y < lines; y++) {
		for (uint8_t x = 0; x < width; x++) {
			uint8_t bit = (buffer[x] & (1 << y)) ? 1 : 0;
			shiftout(bit);
		}

		digitalWrite(13, 1);
		digitalWrite(line_pins[y], 1);
		delayMicroseconds(200);
		digitalWrite(13, 0);
		digitalWrite(line_pins[y], 0);
	}

	ticks++;
	if (ticks == 8) {
		ticks = 0;
		runde--;

		letters(runde);

		if (runde == -(6 * (int)message.length())) {
			runde = 120;
		}
	}
	receive_serial();
}
