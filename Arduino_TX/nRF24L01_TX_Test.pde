/*
 Arduino nRF24L01 TX Test
 Version: 0.2
 Author: Alex from Inside Gadgets (http://www.insidegadgets.com)
 Created: 15/08/2012
 Last Modified: 3/04/2013
 
 Test the receiver on an nRF24L01 with the ATtiny using 
 the Arduino environment
 
 / ATtiny25/45/85 Pin map
 //                                 +-\/-+
 //          Reset/Ain0 (D 5) PB5  1|o   |8  Vcc
 //  nRF24L01 CE - Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1 - nRF24L01 SCK
 // nRF24L01 CSN - Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1 - nRF24L01 MO
 //                           GND  4|    |5  PB0 (D 0) pwm0 - nRF24L01 MI
 //                                 +----+
 
 */

#include <avr/io.h>
#include <util/delay.h>
#include "mirf.h"
#include "nRF24L01.h"

// Defines for setting the MiRF registers for transmitting or receiving mode
#define TX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) )
#define RX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) )

// Flag which denotes transmitting or receiving mode
volatile uint8_t PMODE;

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

// Initializes pins ans interrupt to communicate with the MiRF module
// Should be called in the early initializing phase at startup.
void mirf_init(void) {
  // Define CSN and CE as Output and set them to default
  pinMode (CSN, OUTPUT);
  pinMode (CE, OUTPUT);

  mirf_CE_lo;
  mirf_CSN_hi;
}

// Sets the important registers in the MiRF module and powers the module
// in receiving mode
void mirf_config(void) {
  // Set RF channel
  mirf_config_register(RF_CH, mirf_CH);

  // Set length of incoming payload 
  mirf_config_register(RX_PW_P0, mirf_PAYLOAD);

  // Set RADDR and TADDR
  mirf_write_register(RX_ADDR_P0, RADDR, 5);
  mirf_write_register(TX_ADDR, TADDR, 5); // Not needed for receiver

  // Only enable RX_ADDR_P0 address matching
  mirf_config_register(EN_RXADDR, 1<<ERX_P0);

  // Start receiver 
  PMODE = RXMODE; // Start in receiving mode
  TX_POWERUP;     // Power up in transmitting mode
}

// Flush RX and TX FIFO
void mirf_flush_rx_tx(void) {
  mirf_CSN_lo; // Pull down chip select
  spi_transfer(FLUSH_RX); // Flush RX
  mirf_CSN_hi; // Pull up chip select

  mirf_CSN_lo; // Pull down chip select
  spi_transfer(FLUSH_TX);  // Write cmd to flush tx fifo
  mirf_CSN_hi; // Pull up chip select
}

// Read the status register
uint8_t mirf_status(void) {
  mirf_CSN_lo; // Pull down chip select
  spi_transfer(R_REGISTER | (REGISTER_MASK & STATUS));
  uint8_t status =  spi_transfer(NOP); // Read status register
  mirf_CSN_hi; // Pull up chip select
  return status;
}

// Checks if data is available for reading
uint8_t mirf_data_ready(void) {
  mirf_CSN_lo;                                // Pull down chip select
  spi_transfer(R_REGISTER | (REGISTER_MASK & STATUS));
  uint8_t status = spi_transfer(0x00);        // Read status register
  mirf_CSN_hi;                                // Pull up chip select
  return status & (1<<RX_DR);
}

// Checks if MAX_RT has been reached
uint8_t mirf_max_rt_reached(void) {
  mirf_CSN_lo; // Pull down chip select
  spi_transfer(R_REGISTER | (REGISTER_MASK & STATUS));
  uint8_t status = spi_transfer(NOP); // Read status register
  mirf_CSN_hi; // Pull up chip select
  return status & (1<<MAX_RT);
}

// Reads mirf_PAYLOAD bytes into data array
void mirf_get_data(uint8_t *data) {
  mirf_CSN_lo;                               // Pull down chip select
  spi_transfer(R_RX_PAYLOAD);            // Send cmd to read rx payload
  spi_read_data(data, mirf_PAYLOAD); // Read payload
  mirf_CSN_hi;                               // Pull up chip select
  mirf_config_register(STATUS,(1<<RX_DR));   // Reset status register
}

// Write one byte into the given MiRF register
void mirf_config_register(uint8_t reg, uint8_t value) {
  mirf_CSN_lo;
  spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
  spi_transfer(value);
  mirf_CSN_hi;
}

// Reads an array of bytes from the given start position in the MiRF registers.
void mirf_read_register(uint8_t reg, uint8_t *value, uint8_t len) {
  mirf_CSN_lo;
  spi_transfer(R_REGISTER | (REGISTER_MASK & reg));
  spi_read_data(value, len);
  mirf_CSN_hi;
}

// Writes an array of bytes into inte the MiRF registers
void mirf_write_register(uint8_t reg, uint8_t *value, uint8_t len) {
  mirf_CSN_lo;
  spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
  spi_write_data(value, len);
  mirf_CSN_hi;
}

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void mirf_send(uint8_t *value, uint8_t len) {
  PMODE = TXMODE; // Set to transmitter mode
  TX_POWERUP; // Power up

  mirf_CSN_lo; // Pull down chip select
  spi_transfer(FLUSH_TX);  // Write cmd to flush tx fifo
  mirf_CSN_hi; // Pull up chip select

  mirf_CSN_lo;  // Pull down chip select
  spi_transfer(W_TX_PAYLOAD); // Write cmd to write payload
  spi_write_data(value, len); // Write payload
  mirf_CSN_hi; // Pull up chip select

  mirf_CE_hi; // Start transmission
  _delay_us(15);
  mirf_CE_lo;
}

void setup() {
  // Define CSN and CE as Output and set them to default
  DDRB |= ((1<<CSN)|(1<<CE));
  mirf_CE_lo;
  mirf_CSN_hi;
  
  mirf_init();
  _delay_ms(50);
  mirf_config(); 
}

void loop() {
  uint8_t buffer[5] = {
    14, 123, 63, 23, 54  };
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


