/*
 ATtiny85 SPI
 Version: 1.0
 Author: Alex from Inside Gadgets (http://www.insidegadgets.com)
 Created: 6/05/2012
 Last Modified: 15/08/2012

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
	DDRB |= (1<<PB2); // SPI CLK
	DDRB |= (1<<PB1); // SPI DO
	DDRB &= ~(1<<PB0); // SPI DI
	PORTB |= (1<<PB0); // SPI DI
}

// Setup the ATtiny
void setup(void) {
	spi_init(); // Initialise SPI
}
