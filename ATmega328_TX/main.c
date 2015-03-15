/*
 PIR Wifi Sensor TX
 Version: 0.1
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 28/12/2012
 Last Modified: 3/01/2013
 
 Monitors the output of a PIR, if it goes high we transmit via the nRF24L01 our client code (pir1c) to a server.
 
 */

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

uint8_t data_out[5] = "pir1c";

int main(void) {
	setup();
	mirf_init();
	_delay_ms(50);
	mirf_config();
	
	while(1) {
		
		_delay_ms(1000);
		
		// Let server know the PIR was triggered
		mirf_transmit_data();
		
		_delay_ms(500);
	}
}
