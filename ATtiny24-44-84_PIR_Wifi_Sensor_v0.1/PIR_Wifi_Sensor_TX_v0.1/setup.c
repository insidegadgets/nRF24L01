/*
 PIR Wifi Sensor TX
 Version: 0.1
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 28/12/2012
 Last Modified: 3/01/2013

 */
 
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef boolean
	typedef uint8_t boolean;
#endif
#ifndef bool
	typedef uint8_t bool;
#endif 
#ifndef byte
	typedef uint8_t byte;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define LOW 0
#define HIGH 1
#define false 0
#define true 1

#define T16MS 0
#define T32MS 1
#define T64MS 2
#define T128MS 3
#define T250MS 4
#define T500MS 5
#define T1S 6
#define T2S 7
#define T4S 8
#define T8S 9

// SPI transfer 1 byte and return the result
uint8_t spi_transfer(uint8_t data) {
	USIDR = data;
	USISR = _BV(USIOIF); // clear flag

	while ( (USISR & _BV(USIOIF)) == 0 ) {
		USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
	}
	return USIDR;
}

// Write data using SPI
void spi_write_data(uint8_t * dataout, uint8_t len) {
	uint8_t i;
	for (i = 0; i < len; i++) {
		spi_transfer(dataout[i]);
	}
}

// Read data using SPI
void spi_read_data(uint8_t * datain, uint8_t len) {
	uint8_t i;
	for (i = 0; i < len; i++) {
		datain[i] = spi_transfer(0x00);
	}
}

// Initialise the SPI
void spi_init(void) {
	DDRA |= (1<<PA4); // SPI CLK
	DDRA |= (1<<PA5); // SPI DO
	DDRA &= ~(1<<PA6); // SPI DI
	PORTA |= (1<<PA6); // SPI DI
}


// Used from http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void system_sleep(void) {
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
	sleep_mode(); // System sleeps here
}

// Watchdog timeout values
// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms, 6=1sec, 7=2sec, 8=4sec, 9=8sec
// Used from http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setup_watchdog(byte ii) {
	byte bb;
	if (ii > 9 ) ii=9;
	bb=ii & 7;
	if (ii > 7) bb|= (1<<5);
	bb|= (1<<WDCE);
	
	MCUSR &= ~(1<<WDRF);
	// Start timed sequence
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	// Set new watchdog timeout value
	WDTCSR = bb;
	WDTCSR |= _BV(WDIE);
}

// Turn off the Watchdog
void turnOffwatchdog(void) {
	cli(); // Turn interrupts off
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = 0;
	sei(); // Turn interrupts back on
}

// Setup the ATtiny
void setup(void) {
	spi_init(); // Initialise SPI
	
	// Turn on pull-up resistors on other ports to save power
	PORTB |= ((1<<PB1) | (1<<PB0)); 
	PORTA |= ((1<<PA2) | (1<<PA1) | (1<<PA0)); 
	
	// Initialise the pin change for power loss detection
	sbi(GIMSK, PCIE0); // Enable pin change interrupt
	sbi(PCMSK0, PCINT3); // Apply interrupt to button pin
	
	sei(); // Turn on interrupts
}
