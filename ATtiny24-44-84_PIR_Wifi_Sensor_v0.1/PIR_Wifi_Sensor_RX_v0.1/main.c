/*
 PIR Wifi Sensor RX
 Version: 0.1
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 28/12/2012
 Last Modified: 3/01/2013
 
 Listens for the client's code (pir1c) and if it matches it means the PIR was triggered so turn on an LED/Buzzer 
 for 4 seconds.
 
 */
 
// ATtiny24/44/84 Pin map
//
//               +-\/-+
//         VCC  1|o   |14  GND
//         PB0  2|    |13  PA0
//         PB1  3|    |12  PA1
//   Reset PB3  4|    |11  PA2
//      CE PB2  5|    |10  PA3 LED/Buzzer
//     CSN PA7  6|    |9   PA4 SCK
//      MI PA6  7|    |8   PA5 MO
//               +----+
  
#define F_CPU 1000000 // 1 MHz

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <string.h>
#include "setup.c"
#include "mirf.c"

#define pirPin PA3
uint8_t data_in[5];

int main(void) {
	setup();
	mirf_init();
	_delay_ms(50);
	mirf_config();
	
	while(1) {
		RX_POWERUP;
		mirf_CSN_lo;
		spi_transfer(FLUSH_RX);
		mirf_CSN_hi; 
		mirf_CE_hi; // Start listening
		
		// Wait for incoming requests
		while (!(mirf_status() & (1<<RX_DR))) { 
			_delay_us(250);
		}
		mirf_CE_lo; // Stop listening
		
		// Read the data received
		mirf_receive_data();
		
		// Check if the data_in matches "pir1c"
		if (memcmp(data_in, "pir1c", 5) == 0) {
			PORTA |= (1<<PA3); // Turn on LED/Buzzer
			
			// Start/Restart the 8 second count
			TCNT1 = 0;
			TCCR1B = ((1<<CS11) | (1<<CS10));
			
		}
	}
}

ISR(TIM1_OVF_vect) {
	PORTA &= ~(1<<PA3); // Turn off LED/Buzzer
	TCCR1B = 0; // Turn off Timer
}
