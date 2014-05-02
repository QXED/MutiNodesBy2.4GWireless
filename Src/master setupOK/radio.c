/*============================================================================
PROJECT...............: Wireless Headset
FILE..................:radio.c
INITIAL AUTHOR........: Fly.Zeng GoerTek
INITIAL DATE..........: 18.10.2007
COMPILER..............: Keil uVision3

Description : Functions for initialization and interaction with nRF24L01.


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version			  
============================================================================*/

/***************************************************************************
* Library Includes
***************************************************************************/
#include "stm32f10x_lib.h"

/***************************************************************************
* Local Includes
***************************************************************************/
#include "radio.h"
#include "mcu.h"
#include "protocol.h"
extern char temp;
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

char  read_rf_reg(char address)
{
char data_byte=0;
	ENABLE_RF_SPI;
	spi_byte(R_REG|address);
	data_byte = spi_byte(0xaa);
	DISABLE_RF_SPI;
	return data_byte;
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
#ifdef TEST
void read_rf_data(char inst,char *data_ptr,char nbr_of_bytes)
{
int i;
	ENABLE_RF_SPI;
	spi_byte(inst);
	for(i=0; i<nbr_of_bytes;i++)
		{*(data_ptr)=spi_byte(0x55);
		data_ptr++;
		}
	DISABLE_RF_SPI;
}
#endif
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
	char buffer[3]={0x55,0x55,0x55};
	
	init_spi_rf();

	write_rf_reg(CONFIG,0xec);				// All Interrupts OFF, Mode=Powerdown, 2-byte CRC 
	write_rf_reg(EN_AA,0x00);					// Disable Auto Acknowledge
	write_rf_reg(EN_RXADDR,0x01);				// Enable Datapipe0
	write_rf_reg(SETUP_AW,0x01);				// Set Address width to 3 byte
	write_rf_reg(RF_SETUP,0x0f);				// Bitrate=2Mbps, Output power=0dBm
	
	write_rf_data(W_REG|RX_ADDR_P0,rf_address[TOTAL_ADDR_NUM-1],ADDRESS_LENGTH);		// Set Rx and Tx addresses equal to ID
	
	write_rf_data(W_REG|TX_ADDR,rf_address[TX_ADDR_NUM-1],ADDRESS_LENGTH);

#ifdef TEST	
	read_rf_data(R_REG|RX_ADDR_P0,buffer,3);
#endif
	
	write_rf_reg(RX_PW_P0,4);
	
	clear_rf_interrupt();

	flush_rf_rx();									// Flush RX FIFO
	flush_rf_tx();									// Flush TX FIFO
}

/***************************************************************************
Declaration : void init_rf_signalling(void)

Description : Initializes the nRF24L01 for Call Setup Packets
***************************************************************************/
void init_rf_signalling(void)
{
	write_rf_reg(CONFIG,0xec);				// Set 2-byte CRC
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
		id[0] = 0x42;
		id[1] = 0xce;
		id[2] = 0x10;
	#endif

	write_rf_reg(CONFIG,0xe0);				// All Interrupts OFF, Mode=Powerdown, CRC Disabled 
	write_rf_reg(EN_AA,0x00);					// Disable Auto Acknowledge
	write_rf_reg(EN_RXADDR,0x01);				// Enable Datapipe0
	write_rf_reg(SETUP_AW,0x01);				// Set Address width to 3 byte
	#ifdef TEST_TX_CW
		write_rf_reg(RF_SETUP,0x1f);			// Bitrate=2Mbps, Output power=0dBm, Force PLL-Lock
	#else
		write_rf_reg(RF_SETUP,0x0f);			// Bitrate=2Mbps, Output power=0dBm	
	#endif
	write_rf_data(W_REG|RX_ADDR_P0,&id[0],3);		// Set Rx and Tx addresses equal to ID
	write_rf_data(W_REG|TX_ADDR,&id[0],3);
	write_rf_reg(RX_PW_P0,32);						// 32-byte payload
	clear_rf_interrupt();

	flush_rf_rx();									// Flush RX FIFO
	flush_rf_tx();									// Flush TX FIFO
}


/***************************************************************************
Declaration : void init_rf(void)

Description : Initialize nRF24L01
***************************************************************************/
void init_rf_esb(void)
{
char id[3];
char cTmp=0x73;
	init_spi_rf();
	#ifdef TEST_TX_CW
		id[0] = 0;
		id[1] = 0;
		id[2] = 0;
	#else
		id[0] = 0x42;
		id[1] = 0xce;
		id[2] = 0x10;
	#endif
	write_rf_reg(CONFIG,0xec);				// All Interrupts OFF, Mode=Powerdown, CRC Disabled 
	write_rf_reg(EN_AA,0x01);					// Enable Auto Acknowledge

  	write_rf_reg(EN_RXADDR,0x01);				// Enable Datapipe0
	write_rf_reg(SETUP_AW,0x01);				// Set Address width to 3 byte
	write_rf_reg(RF_SETUP,0x07);			// Bitrate=2Mbps, Output power=0dBm	
	write_rf_data(W_REG|RX_ADDR_P0,&id[0],3);		// Set Rx and Tx addresses equal to ID
	write_rf_data(W_REG|TX_ADDR,&id[0],3);
	write_rf_reg(RX_PW_P0,32);						// 32-byte payload
//	write_rf_reg(RX_PW_P0,4);			
//	write_rf_reg(SETUP_RETR, 0x24); 			// 500us + 86us, 10 retrans.

	if(read_rf_reg(FEATURE)!=0x06 || read_rf_reg(DYNPD)!=0x01)
		{//write_rf_data(ACTIVATE,&cTmp,1);
		write_rf_reg(FEATURE,0x06);	
		write_rf_reg(DYNPD,0x01);
		}	

	write_rf_data(ACTIVATE,&cTmp,1);					// 32-byte payload
	if(read_rf_reg(FEATURE)!=0x06 || read_rf_reg(DYNPD)!=0x01)
		{//write_rf_data(ACTIVATE,&cTmp,1);
		write_rf_reg(FEATURE,0x06);	
		write_rf_reg(DYNPD,0x01);
		}
			
    if(read_rf_reg(FEATURE)!=0x06)
		write_rf_data(ACTIVATE,&cTmp,1);
  //	cTmp=read_rf_reg(FEATURE);
   // write_rf_data(ACTIVATE,&cTmp,1);
  //  write_rf_data(ACTIVATE,&cTmp,1);
	clear_rf_interrupt();
	flush_rf_rx();									// Flush RX FIFO
	flush_rf_tx();
}

/***************************************************************************
Declaration : void set_rf_rx(char freq)

Description : Set nRF24L01 in RX mode at the given RF Channel
***************************************************************************/
void set_rf_rx(char freq)
{
	write_rf_reg(CONFIG,0x3b);				// RX_DR_INT=ON, Mode=PRX 	
	write_rf_reg(RF_CH,freq);
}

/***************************************************************************
Declaration : void set_rf_tx(char freq)

Description : Set nRF24L01 in TX mode at the given RF Channel
***************************************************************************/
void set_rf_tx(char freq)
{

	write_rf_reg(CONFIG,0x5a);				// TX_DR_INT=ON, Mode=PTX 	
	write_rf_reg(RF_CH,freq);
//	temp=read_rf_reg(RF_CH);
}

void set_rf_txesb(char freq)
{

	write_rf_reg(CONFIG,0x3e);				// RX_DR_INT=ON,, Mode=PTX 	
	write_rf_reg(RF_CH,freq);
//	temp=read_rf_reg(RF_CH);
}
/***************************************************************************
Declaration : void set_rf_off(void)

Description : Set nRF24L01 in TX mode at the given RF Channel
***************************************************************************/
void set_rf_off(void)
{
	write_rf_reg(CONFIG,0xe8);				// All Interrupts OFF, Mode=Powerdown	
}

/***************************************************************************
Declaration : void clear_rf_interrupt(void)

Description : Clear all interrupts from nRF24L01.
***************************************************************************/
void clear_rf_interrupt(void)
{
// while(0x00 != (0xf0 & read_rf_reg(RF_STATUS)))
	   write_rf_reg(RF_STATUS,0xf0);				// Clear all interrupts	
}

/***************************************************************************
Declaration : void rf_enable_pulse(void)

Description : Generates a 10us chip enable pulse to nRF24L01
***************************************************************************/
void rf_enable_pulse(void)
{
	RF_ENABLE;										// Set chip enable
	start_timer4(RF_ENABLE_WIDTH);		
//	LED_TOGGLE;
//    temp=TIM_GetCounter(TIM4);
	while(!TIMER4_TIMEOUT)							// Wait for Timer0 timeout
	    ;
//	temp=TIM_GetCounter(TIM4);
//	LED_TOGGLE;

	RF_DISABLE;
	// Clear chip enable
}

/***************************************************************************
Declaration : char wait_rf_irq(char timeout, char prescaler)

Description : Waits for RF Interrupt or Timeout
***************************************************************************/
char wait_rf_irq(unsigned short  timeout, char  prescaler)
{
	start_timer4(timeout);
	while(1)
	{
		if(TIMER4_TIMEOUT)
			return(STATUS_TIMEOUT);
		if(!nRF_IRQ)
			return(STATUS_RF_READY);
	}
}
