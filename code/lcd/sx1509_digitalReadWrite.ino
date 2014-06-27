/*
 * SX1509 Library Example 01 by: Jim Lindblom SparkFun Electronics license:
 * Beerware. Please use, reuse, share, and modify this code. I'd ask that you
 * maintain attribution and open-source. If you find it useful, you can buy
 * me a beer when we meet some day.
 * 
 * This is the simplest example for the SX1509 Arduino library. This sketch
 * shows how you can use the following SX1509 library   methods: constructor
 * - initialize an instance of the SX1509 library. The minimum required
 * parameter is the I2C address of the SX1509. Other parameters (resetPin,
 * interruptPin, oscPin) are optional (as shown). init() - ializes the
 * SX1509. Will perform a reset, start up the Wire library and read some
 * registers to make sure the SX1509 is operational. pinDir(pin,
 * INPUT/OUTPUT) - This method functions much like the Arduino pinMode()
 * function. pin should be an SX1509 pin between 0 and 15. INPUT and OUTPUT
 * work like usual. writePin(pin, HIGH/LOW) - Similar to the Arduino
 * digitalWrite() function. pin should be between 0 and 15, HIGH and LOW work
 * as usual. This function will also activate the pull-up and down resistors
 * if the pin is configured as an input. readPin(pin) - This function works
 * like the digitalRead() function of the Arduino. Give it a pin between 0
 * and 15 and a 0 or 1 will be returned representing whether the pin is LOW
 * or HIGH.
 * 
 * Hardware: The SX1509 should be hooked up like so: SX1509 Pin      Arduino Pin
 * 3.3V ---------- 3.3V GND ----------- GND SDA ----------- A4 (or SDA on
 * newer boards) SCL ----------- A5 (or SCL on newer boards)
 * 
 * nRST, OSCIO, and nINT are not used in this example. Though they can be
 * connected to any digital pin.
 * 
 * See the SX1509_ADDRESS defines to decide which address you need to send to
 * the constructor. By default the SX1509 Breakout sets both ADDR pins to 0
 * (so 0x3E I2C address).
 * 
 * In addition two SX1509 i/o pins are used: 14 - Output, could be connected to
 * an LED 1 - Input, could be connected to a button.
 */

#include <Wire.h>  // Wire.h library is required to use SX1509 lib
#include <sx1509_library.h>  // Include the SX1509 library
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "LiquidCrystal.h"

// Uncomment one of the four lines to match your SX1509 s address
// pin selects.SX1509 breakout defaults to[0:0] (0x3E).
const byte	SX1509_ADDRESS = 0x3E;
//SX1509 I2C address(00)
	// const byte	SX1509_ADDRESS = 0x3F;
//SX1509 I2C address(01)
	// const byte	SX1509_ADDRESS = 0x70;
//SX1509 I2C address(10)
	// const byte	SX1509_ADDRESS = 0x71;
//SX1509 I2C address(11)
// Pin definitions, not actually used in this exampl

// Create a new sx1509Class object.You can make it with all
// of the above pins:
//sx1509Class sx1509(SX1509_ADDRESS, resetPin, interruptPin);
//Or make an sx1509 object with just the SX1509 I2C address:
sx1509Class sx1509(SX1509_ADDRESS);

	uint8_t		_displayfunction;
	uint8_t		_displaycontrol;
	uint8_t		_displaymode;

	uint8_t		_initialized;

	uint8_t		_numlines, _currline;


//SX1509 pin defintions:j


	// const byte	D4 = 8;
	//const byte	D5 = 9;
	//const byte	D6 = 10;
	//const byte	D7 = 11;

	const byte	RS = 0;
	const byte	E = 1;

	const byte	D4 = 2;
	const byte	D5 = 3;
	const byte	D6 = 4;
	const byte	D7 = 5;

	const byte	LED = 6;


	void		lcd_blink ()
{
	_displaycontrol |= LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void
setup()
{
	Serial.begin(9600);
	sx1509.init();
	//Initialize the SX1509, does Wire.begin()
	sx1509.pinDir(RS, OUTPUT);
	sx1509.pinDir(E, OUTPUT);

	sx1509.pinDir(D4, OUTPUT);
	sx1509.pinDir(D5, OUTPUT);
	sx1509.pinDir(D6, OUTPUT);
	sx1509.pinDir(D7, OUTPUT);
	sx1509.pinDir(LED, OUTPUT);

	initialize();
	sx1509.writePin(LED, HIGH);
	lcd_blink();

}

void
pulseEnable(void)
{
	sx1509.writePin(E, LOW);
	delayMicroseconds(1);
	sx1509.writePin(E, HIGH);
	delayMicroseconds(1);
	sx1509.writePin(E, LOW);
	delayMicroseconds(100);
}

void
lcd_write4bits(unsigned char x)
{
	sx1509.writePin(D7, ((x & 0x08) == 0x08) ? HIGH : LOW);
	sx1509.writePin(D6, ((x & 0x04) == 0x04) ? HIGH : LOW);
	sx1509.writePin(D5, ((x & 0x02) == 0x02) ? HIGH : LOW);
	sx1509.writePin(D4, ((x & 0x01) == 0x01) ? HIGH : LOW);
	pulseEnable();
}


void
lcd_send(unsigned char value, unsigned char mode)
{
	sx1509.writePin(RS, (mode == LOW) ? LOW : HIGH);
	lcd_write4bits(value >> 4);
	lcd_write4bits(value);

}

void
lcd_command(unsigned char value)
{
	lcd_send(value, LOW);
}

size_t
lcd_write(unsigned char value)
{
	lcd_send(value, HIGH);
	return 1;
}

void
display()
{
	_displaycontrol |= ~LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void
lcd_clear()
{
	lcd_command(LCD_CLEARDISPLAY);
	delayMicroseconds(2000);
}

void
initialize()
{
	_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	//turn the display on with no cursor or blinking default
		_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	_currline = 0;
	_numlines = 2;
	delayMicroseconds(500000);
	sx1509.writePin(RS, LOW);
	sx1509.writePin(E, LOW);
	//this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46

		// we start in 8 bit mode, try to set 4 bit mode
	lcd_write4bits(0x03);
	delayMicroseconds(4500);
	//wait min 4.1 ms

        // second try
	lcd_write4bits(0x03);
	delayMicroseconds(4500);
	//wait min 4.1 ms

		// third go !
		lcd_write4bits(0x03);
	delayMicroseconds(150);

	//finally, set to 4 - bit interface
		lcd_write4bits(0x02);

	lcd_command(LCD_FUNCTIONSET | _displayfunction);

	display();

	//clear it off
		lcd_clear();

	//Initialize to default text direction(for romance languages)
		_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	//set the entry mode
		lcd_command(LCD_ENTRYMODESET | _displaymode);

}

void
loop()
{
	//int		buttonValue = sx1509.readPin(buttonPin);
	//read pin 1 status
	lcd_blink();
	int		i;
	lcd_clear();


	unsigned char	stuff;
	//lcd_write(i);

	stuff = -1;

	for (i = 0; i < 255; i++) {


		//while (Serial.available() <= 0);
		//stuff = Serial.read();
		lcd_write(i);
		Serial.print(i, HEX);
		if (i % 16 == 0) {
			lcd_clear();
			Serial.print("\n");
		}
	}
	delayMicroseconds(1000000);

	lcd_clear();
	Serial.write("loop\n");
}

