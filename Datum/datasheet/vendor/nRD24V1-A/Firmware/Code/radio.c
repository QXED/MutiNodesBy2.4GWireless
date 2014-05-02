/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: radio.c
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : Functions for initialization and interaction with nRF24L01.


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
06.12.2006		1.1			Test SW includes Modulated Transmitter				  
============================================================================*/

/***************************************************************************
* Library Includes
***************************************************************************/
#include <avr/io.h>

/***************************************************************************
* Local Includes
***************************************************************************/
#include "radio.h"
#include "mcu.h"
#include "protocol.h"

/***************************************************************************
* Constants
***************************************************************************/

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

/***************************************************************************
Declaration : char read_rf_byte(char byte)

Description : Reads one byte from the nRF24L01
***************************************************************************/
char read_rf_byte(char byte)
{
	return(spi_byte(byte));
}

/***************************************************************************
Declaration : void write_rf_byte(char byte)

Description : Writes one byte to the nRF24L01
***************************************************************************/
void write_rf_byte(char byte)
{
	spi_byte(byte);
}

/***************************************************************************
Declaration : void write_rf_reg(char address,char data_byte)

Description : Writes one byte of data to the RF register address
***************************************************************************/
void write_rf_reg(char address,char data_byte)
{
	ENABLE_RF_SPI;
	spi_byte(W_REG|address);
	spi_byte(data_byte);
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void write_rf_data(char inst,char *data_ptr,char nbr_of_bytes)

Description : Writes the instruction and the number of bytes of data.
***************************************************************************/
void write_rf_data(char inst,char *data_ptr,char nbr_of_bytes)
{
int i;

	ENABLE_RF_SPI;
	spi_byte(inst);
	for(i=0; i<nbr_of_bytes;i++)
		spi_byte(*data_ptr++);
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void flush_rf_tx(void)

Description : Flush the radio TX FIFO.
***************************************************************************/
void flush_rf_tx(void)
{
	ENABLE_RF_SPI;
	spi_byte(FLUSH_TX);
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void flush_rf_rx(void)

Description : Flush the radio RX FIFO.
***************************************************************************/
void flush_rf_rx(void)
{
	ENABLE_RF_SPI;
	spi_byte(FLUSH_RX);
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void reuse_rf_payload(void)

Description : Set the radio to reuse the payload for following transmissions.
***************************************************************************/
void reuse_rf_payload(void)
{
	ENABLE_RF_SPI;
	spi_byte(REUSE_TX_PL);
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void init_rf(void)

Description : Initialize nRF24L01
***************************************************************************/
void init_rf(void)
{
char id[3];

	init_spi_rf();
	
	// Read ID from EEPROM
	id[0] = eeprom_read(EEPROM_ADR_ID0);
	id[1] = eeprom_read(EEPROM_ADR_ID1);
	id[2] = eeprom_read(EEPROM_ADR_ID2);
	
	write_rf_reg(CONFIG,0b01111100);				// All Interrupts OFF, Mode=Powerdown, 2-byte CRC 
	write_rf_reg(EN_AA,0b00000000);					// Disable Auto Acknowledge
	write_rf_reg(EN_RXADDR,0b00000001);				// Enable Datapipe0
	write_rf_reg(SETUP_AW,0b00000001);				// Set Address width to 3 byte
	write_rf_reg(RF_SETUP,0b00001111);				// Bitrate=2Mbps, Output power=0dBm
	write_rf_data(W_REG|RX_ADDR_P0,&id[0],3);		// Set Rx and Tx addresses equal to ID
	write_rf_data(W_REG|TX_ADDR,&id[0],3);
	write_rf_reg(RX_PW_P0,32);						// 32-byte payload
	flush_rf_rx();									// Flush RX FIFO
	flush_rf_tx();									// Flush TX FIFO
}

/***************************************************************************
Declaration : void init_rf_signalling(void)

Description : Initializes the nRF24L01 for Call Setup Packets
***************************************************************************/
void init_rf_signalling(void)
{
	write_rf_reg(CONFIG,0b01111100);				// Set 2-byte CRC
	write_rf_reg(RX_PW_P0,3);						// 3-byte payload	
}

/***************************************************************************
Declaration : void init_rf(void)

Description : Initialize nRF24L01
***************************************************************************/
void init_rf_test(void)
{
char id[3];

	init_spi_rf();
	#ifdef TEST_TX_CW
		id[0] = 0;
		id[1] = 0;
		id[2] = 0;
	#else
		id[0] = 0xAA;
		id[1] = 0xAA;
		id[2] = 0xAA;
	#endif

	write_rf_reg(CONFIG,0b01110000);				// All Interrupts OFF, Mode=Powerdown, CRC Disabled 
	write_rf_reg(EN_AA,0b00000000);					// Disable Auto Acknowledge
	write_rf_reg(EN_RXADDR,0b00000001);				// Enable Datapipe0
	write_rf_reg(SETUP_AW,0b00000001);				// Set Address width to 3 byte
	#ifdef TEST_TX_CW
		write_rf_reg(RF_SETUP,0b00011111);			// Bitrate=2Mbps, Output power=0dBm, Force PLL-Lock
	#else
		write_rf_reg(RF_SETUP,0b00001111);			// Bitrate=2Mbps, Output power=0dBm	
	#endif
	write_rf_data(W_REG|RX_ADDR_P0,&id[0],3);		// Set Rx and Tx addresses equal to ID
	write_rf_data(W_REG|TX_ADDR,&id[0],3);
	write_rf_reg(RX_PW_P0,32);						// 32-byte payload
	flush_rf_rx();									// Flush RX FIFO
	flush_rf_tx();									// Flush TX FIFO
}

/***************************************************************************
Declaration : void set_rf_rx(char freq)

Description : Set nRF24L01 in RX mode at the given RF Channel
***************************************************************************/
void set_rf_rx(char freq)
{
	write_rf_reg(CONFIG,0b00111011);				// RX_DR_INT=ON, Mode=PRX 	
	write_rf_reg(RF_CH,freq);
}

/***************************************************************************
Declaration : void set_rf_tx(char freq)

Description : Set nRF24L01 in TX mode at the given RF Channel
***************************************************************************/
void set_rf_tx(char freq)
{
	write_rf_reg(CONFIG,0b01011010);				// TX_DR_INT=ON, Mode=PTX 	
	write_rf_reg(RF_CH,freq);
}

/***************************************************************************
Declaration : void set_rf_off(void)

Description : Set nRF24L01 in TX mode at the given RF Channel
***************************************************************************/
void set_rf_off(void)
{
	write_rf_reg(CONFIG,0b01111000);				// All Interrupts OFF, Mode=Powerdown	
}

/***************************************************************************
Declaration : void clear_rf_interrupt(void)

Description : Clear all interrupts from nRF24L01.
***************************************************************************/
void clear_rf_interrupt(void)
{
	write_rf_reg(RF_STATUS,0b01110000);				// Clear all interrupts	
}

/***************************************************************************
Declaration : void rf_enable_pulse(void)

Description : Generates a 10us chip enable pulse to nRF24L01
***************************************************************************/
void rf_enable_pulse(void)
{
	RF_ENABLE;										// Set chip enable
	start_timer0(RF_ENABLE_WIDTH,DIV1);				// Init Timer0
	while(!TIMER0_TIMEOUT)							// Wait for Timer0 timeout
		;
	RF_DISABLE;										// Clear chip enable
}

/***************************************************************************
Declaration : char wait_rf_irq(char timeout, char prescaler)

Description : Waits for RF Interrupt or Timeout
***************************************************************************/
char wait_rf_irq(char timeout, char prescaler)
{
	start_timer0(timeout,prescaler);
	while(1)
	{
		if(TIMER0_TIMEOUT)
			return(STATUS_TIMEOUT);
		if(!nRF_IRQ)
			return(STATUS_RF_READY);
	}
}
