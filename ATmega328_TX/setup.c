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
	SPDR = data;
	
	while(!(SPSR & (1<<SPIF)));

	return SPDR;
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
	DDRB |= (1<<PB5); // SPI CLK
	DDRB |= (1<<PB3); // SPI DO
	DDRB &= ~(1<<PB4); // SPI DI
	PORTB |= (1<<PB4); // SPI DI
	
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

// Setup the ATtiny
void setup(void) {
	sei(); // Turn on interrupts
}
