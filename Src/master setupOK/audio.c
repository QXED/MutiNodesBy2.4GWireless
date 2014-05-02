/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: audio.c
INITIAL AUTHOR........: Fly.Zeng GoerTek
INITIAL DATE..........: 18.10.2007
COMPILER..............: Keil uVision3

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
#include "stm32f10x_lib.h"

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


/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/

/***************************************************************************
Declaration : void write_codec_reg(char address,char data_byte)

Description : Writes one byte of data to the Codec register address
***************************************************************************/


/***************************************************************************
Declaration : void init_codec(void)

Description : Initialize the Codec XE3005
***************************************************************************/
void init_codec(void)
{
}

/***************************************************************************
Declaration : void start_codec(void)

Description : Start the Codec Interface
***************************************************************************/
void start_codec(void)
{
	  TIM_Cmd(TIM2, ENABLE);
	  TIM_GenerateEvent(TIM2,TIM_EventSource_Update);
	  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
}

/***************************************************************************
Declaration : void reset_codec(void)

Description : Reset the Codec Interface
***************************************************************************/
void reset_codec(void)
{

}

/***************************************************************************
Declaration : void stop_codec(void)

Description : Stop the Codec Interface
***************************************************************************/
void stop_codec(void)
{
	  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	  TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);
	  TIM_Cmd(TIM2, DISABLE);
}


