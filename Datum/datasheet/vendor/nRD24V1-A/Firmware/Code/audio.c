/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: audio.c
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : Functions for initialization and interaction with audio codec
			  XE3005 and USB audio controller SN11220ACF.

Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
06.11.2006		1.1			Included function reset_codec().
21.11.2006		1.2			Changed reset_codec function				  
============================================================================*/

/***************************************************************************
* Library Includes
***************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>

/***************************************************************************
* Local Includes
***************************************************************************/
#include "audio.h"
#include "main.h"
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
char sample_counter;

/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/
void write_codec_reg(char address,char data_byte);

/***************************************************************************
Interrupt: SIGNAL (SIG_OUTPUT_COMPARE2B)

Function : Interrupt routine that is called when USART databuffer is empty.
		   One byte of data is transmitted/received to/from the Codec.	
***************************************************************************/
#ifndef USB
SIGNAL (SIG_OUTPUT_COMPARE2B)
{
	*input_write_ptr++ = UDR0;
	*input_write_ptr++ = UDR0;
	UDR0 = *output_read_ptr++;
	UDR0 = *output_read_ptr++;

	if(input_write_ptr >= &input[AUDIO_BUFFER_LENGTH])
		input_write_ptr = &input[0];
	if(output_read_ptr >= &output[AUDIO_BUFFER_LENGTH])
		output_read_ptr = &output[0];
}
#endif
/***************************************************************************
Interrupt: SIGNAL (SIG_OUTPUT_COMPARE1A)

Function : Interrupt routine that is called when USART databuffer is empty.
		   One byte of data is transmitted/received to/from the Codec.	
***************************************************************************/
#ifdef USB
SIGNAL (SIG_OUTPUT_COMPARE1A)
{
	*input_write_ptr++ = UDR0;
	UDR0 = *output_read_ptr++;

	if(input_write_ptr >= &input[AUDIO_BUFFER_LENGTH])
		input_write_ptr = &input[0];
	if(output_read_ptr >= &output[AUDIO_BUFFER_LENGTH])
		output_read_ptr = &output[0];
	
	if(sample_counter >= (2*AUDIO_SAMPLES))
	{
		START_TIMER2;
		sample_counter = 0;
	}

	sample_counter++;	
}
#endif
/***************************************************************************
Declaration : void write_codec_reg(char address,char data_byte)

Description : Writes one byte of data to the Codec register address
***************************************************************************/
void write_codec_reg(char address,char data_byte)
{
	ENABLE_CODEC_SPI;
	spi_byte(WRITE_REG|address);
	spi_byte(data_byte);
	DISABLE_CODEC_SPI;
}

/***************************************************************************
Declaration : void init_codec(void)

Description : Initialize the Codec XE3005
***************************************************************************/
void init_codec(void)
{
	#ifdef USB
	
	#else
		DISABLE_CODEC_RESET;
		init_spi_codec();
		write_codec_reg(C_REG,0xF0);				// Fsync < 20kHz 
		write_codec_reg(E_REG,0b00001100);			// Disable VMIC, Gain = 20x
		write_codec_reg(I_REG,0b00001101);			// Disable DA, PA and AD
		write_codec_reg(J_REG,0b00100100);			// Slave Mode using 1'st channel
		init_spi_rf();
	#endif
}

/***************************************************************************
Declaration : void start_codec(void)

Description : Start the Codec Interface
***************************************************************************/
void start_codec(void)
{
	#ifdef USB
		sample_counter = 0;
		start_usart();								// Enable USART
	#else
		init_spi_codec();
		write_codec_reg(E_REG,0b10001100);			// Enable VMIC, Gain = 20x
		write_codec_reg(I_REG,0b00000001);			// Enable DA, PA and AD
		init_spi_rf();
		start_usart();								// Enable USART
	#endif
}

/***************************************************************************
Declaration : void reset_codec(void)

Description : Reset the Codec Interface
***************************************************************************/
void reset_codec(void)
{
	cli();
	STOP_TIMER2;
	if(sample_counter >= (2*(AUDIO_SAMPLES-1)))
		sample_counter -= 2;
	sei();
}

/***************************************************************************
Declaration : void stop_codec(void)

Description : Stop the Codec Interface
***************************************************************************/
void stop_codec(void)
{
	#ifdef USB
		stop_usart();
	#else
		init_spi_codec();
		write_codec_reg(E_REG,0b00001100);			// Disable VMIC, Gain = 20x
		write_codec_reg(I_REG,0b00001101);			// Disable DA, PA and AD
		init_spi_rf();
		stop_usart();								// Disable USART
	#endif
}


