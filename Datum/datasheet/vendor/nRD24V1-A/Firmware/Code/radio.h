/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: radio.h
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : 


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version				  
============================================================================*/
#ifndef RADIO_H
#define RADIO_H

/***************************************************************************
* Library Includes
***************************************************************************/

/***************************************************************************
* Local Includes
***************************************************************************/
#include "application.h"
#include "main.h"

/***************************************************************************
* Constants
***************************************************************************/
// nRF24L01 Instruction Names
#define R_REG			0x00
#define W_REG			0x20
#define R_RX_PAYLOAD	0x61
#define W_TX_PAYLOAD	0xA0
#define FLUSH_TX		0xE1
#define FLUSH_RX		0xE2
#define REUSE_TX_PL		0xE3

// nRF24L01 Register Addresses
#define CONFIG		0x00
#define EN_AA		0x01
#define EN_RXADDR	0x02
#define SETUP_AW	0x03
#define SETUP_RETR	0x04
#define RF_CH		0x05
#define RF_SETUP	0x06
#define RF_STATUS	0x07
#define OBSERVE_TX	0x08
#define CD			0x09
#define RX_ADDR_P0	0x0A
#define RX_ADDR_P1	0x0B
#define RX_ADDR_P2	0x0C
#define RX_ADDR_P3	0x0D
#define RX_ADDR_P4	0x0E
#define RX_ADDR_P5	0x0F
#define TX_ADDR		0x10
#define RX_PW_P0	0x11
#define RX_PW_P1	0x12
#define RX_PW_P2	0x13
#define RX_PW_P3	0x14
#define RX_PW_P4	0x15
#define RX_PW_P5	0x16
#define FIFO_STATUS	0x17

// Timeouts
# define RF_ENABLE_WIDTH	(F_CPU/1000*(10)) // 41 DIV1
		
/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/

/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/
void init_rf(void);
void init_rf_signalling(void);
void set_rf_rx(char freq);
void set_rf_tx(char freq);
void set_rf_off(void);
void rf_clearint(void);
char read_rf_byte(char byte);
void write_rf_byte(char byte);
void write_rf_reg(char address,char data_byte);
void write_rf_data(char inst,char *data_ptr,char nbr_of_bytes);
void reuse_rf_payload(void);
void flush_rf_tx(void);
void flush_rf_rx(void);
void clear_rf_interrupt(void);
void rf_enable_pulse(void);
char wait_rf_irq(char timeout, char prescaler);
void init_rf_test(void);

#endif
