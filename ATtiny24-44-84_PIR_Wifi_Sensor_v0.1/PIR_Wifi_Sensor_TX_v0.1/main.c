/*
 PIR Wifi Sensor TX
 Version: 0.1
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 28/12/2012
 Last Modified: 3/01/2013
 
 Monitors the output of a PIR, if it goes high we transmit via the nRF24L01 our client code (pir1c) to a server.
 
 */
 
// ATtiny24/44/84 Pin map
//
//               +-\/-+
//         VCC  1|o   |14  GND
//         PB0  2|    |13  PA0
//         PB1  3|    |12  PA1
//   Reset PB3  4|    |11  PA2
//      CE PB2  5|    |10  PA3 PIR output
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
uint8_t data_out[5] = "pir1c";

int main(void) {
	setup();
	mirf_init();
	_delay_ms(50);
	mirf_config();
	
	while(1) {
		// Power down the nRF24L01
		POWERDOWN;
		
		// Sleep until a pin change or watchdog wakes us up
		system_sleep();
		turnOffwatchdog();
		
		// Check if the PIR output is high
		if (PINA & (1<<pirPin)) {
			// Power up the nRF24L01
			TX_POWERUP;
			_delay_ms(3);
			
			// Let server know the PIR was triggered
			mirf_transmit_data();
		}
		
		// Turn on the watchdog timer to re-check again in 4 seconds because the PIR output will still be high if movement 
		// is still being detected
		setup_watchdog(T4S);
	}
}

ISR(WDT_vect) { }

ISR(PCINT0_vect) { }
