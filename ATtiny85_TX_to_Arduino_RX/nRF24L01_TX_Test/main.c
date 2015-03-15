/*
 ATtiny25 nRF24L01 TX Test
 Version: 0.2
 Author: Alex from Inside Gadgets (http://www.insidegadgets.com)
 Created: 15/08/2012
 Last Modified: 27/03/2013
 
 Test the transmit on an nRF24L01 with the ATtiny25
 
 */

#define F_CPU 1000000 // 1 MHz

#include <avr/io.h>
#include <util/delay.h>
#include "setup.c"
#include "mirf.c"

// ATtiny25/45/85 Pin map
//                                 +-\/-+
//          Reset/Ain0 (D 5) PB5  1|o   |8  Vcc
//  nRF24L01 CE - Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1 - nRF24L01 SCK
// nRF24L01 CSN - Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1 - nRF24L01 MO
//                           GND  4|    |5  PB0 (D 0) pwm0 - nRF24L01 MI
//                                 +----+

int main(void) {
	setup();
	mirf_init();
	_delay_ms(50);
	mirf_config();
	uint8_t buffer[5] = {14, 123, 63, 23, 54};
	uint8_t buffersize = 5;
	
	while(1) {
		mirf_flush_rx_tx(); // Flush TX/RX
		mirf_send(buffer, buffersize);
		_delay_ms(1000);
		
		// If maximum retries were reached, reset MAX_RT
		if (mirf_max_rt_reached()) {
			mirf_config_register(STATUS, 1<<MAX_RT);
		}
	}
}
