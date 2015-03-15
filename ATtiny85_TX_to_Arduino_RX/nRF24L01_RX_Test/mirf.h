/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

#ifndef _MIRF_H_
#define _MIRF_H_

#include <WConstants.h>

// Mirf settings
#define RXMODE 1
#define TXMODE 0
#define mirf_CH         2
#define mirf_PAYLOAD    5
#define mirf_CONFIG     ( (1<<EN_CRC) | (0<<CRCO) )
#define RADDR		(byte *)"clnt1"
#define TADDR		(byte *)"clnt2"

// Pin definitions for chip select and chip enabled of the MiRF module
#define CE  2
#define CSN 10

// Definitions for selecting and enabling MiRF module
#define mirf_CSN_hi     digitalWrite(CSN, HIGH);
#define mirf_CSN_lo     digitalWrite(CSN, LOW);
#define mirf_CE_hi      digitalWrite(CE, HIGH);
#define mirf_CE_lo      digitalWrite(CE, LOW);

// Public standard functions
extern void mirf_init(void);
extern void mirf_config(void);
extern void mirf_send(uint8_t *value, uint8_t len);
extern uint8_t mirf_data_ready(void);
extern void mirf_get_data(uint8_t *data);

// Public extended functions
extern void mirf_config_register(uint8_t reg, uint8_t value);
extern void mirf_read_register(uint8_t reg, uint8_t *value, uint8_t len);
extern void mirf_write_register(uint8_t reg, uint8_t *value, uint8_t len);

#endif /* _MIRF_H_ */

