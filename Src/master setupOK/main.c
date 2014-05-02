/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: main.c
INITIAL AUTHOR........: Fly.Zeng GoerTek
INITIAL DATE..........: 18.10.2007
COMPILER..............: Keil uVision3

Description : Main application handling MMI and Call control.


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
				  
============================================================================*/


/****************************************************************************
* Library Includes
*****************************************************************************/
#include "stm32f10x_lib.h"

/****************************************************************************
* Local Includes
****************************************************************************/
#include "main.h"
#include "protocol.h"
#include "audio.h"
#include "mcu.h"
#include "radio.h"
#include "sin500.h"

/****************************************************************************
* Constants
****************************************************************************/

/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/
char call_state;
char call_status;
char call_timer;
//char signal_in[2];
char signal_in[SIGNALING_LEN];

char signal_out[2];
char key_code;
char input[AUDIO_BUFFER_LENGTH+4];
//char output[AUDIO_BUFFER_LENGTH+4];
volatile char *input_write_ptr;
//volatile char *output_read_ptr;
char *input_read_ptr;
//char *output_write_ptr;
char freq[2];
char setup_freq[4];
//unsigned short call_activity_timer;
unsigned short frame_loss;
short packet_loss[2];

/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/
void init_buffer(void);
char read_key(void);

/***************************************************************************
Declaration : int main(void)

Function :    Main Loop
***************************************************************************/
int main(void)
{
	init_mcu();
 	init_ctrl();
	init_rf();
	init_buffer();
	init_protocol();
	init_freq();

	/* Main Background loop */
	
	start_timer3();//FRAME_PERIOD
	call_state = CALL_SETUP;

/*
#ifdef TEST

	client_id[0]=CLIENT_ID11;
	client_id[1]=CLIENT_ID22;
	client_id[2]=CLIENT_ID33;
	client_id[3]=CLIENT_ID44;
	client_id[4]=CLIENT_ID44;
	start_codec();
	call_state = CALL_CONNECTED;

#endif	
*/

//	for(i=0;i<10;i++)
//		sleep(WDT_TIMEOUT_250MS,STANDBY_MODE);

	while(1)
	{
		/* Call States */	
		switch (call_state)
		{
			case CALL_IDLE:
			 // sleep(WDT_TIMEOUT_60MS,STANDBY_MODE);
				//call_status = CALL_NO_ACTIVITY;
				
				//if(!CALL_SETUP_KEY)
					call_status = CALL_ACTIVITY;
				if(call_status == CALL_ACTIVITY)
					call_state = CALL_SETUP;

			break;
			
			case CALL_SETUP:

				LED_ON;
				call_status = call_setup(&setup_freq[0],N_FREQ_SETUP);
				LED_OFF;
				if(connectstatus==CALL_SETUP_OK)
				{
					init_buffer();
					init_rf();
					init_protocol();
					start_codec();

					//start_timer3();

					call_state = CALL_CONNECTED;
				}	
				else
					call_state = CALL_IDLE;

			break;
			
			case CALL_CONNECTED:
				// Send and receive audio packet
				data_transfer();
				call_state = CALL_IDLE;
				stop_codec();
				init_protocol();

				break;

			default:
				break;
		}
	}
}

/***************************************************************************
Declaration : void init_buffer(void)

Function :    Initializes audio buffers
***************************************************************************/
void init_buffer(void)
{
int i;
//short int temp=0;
//short int  	data_word;
	for(i=0; i<AUDIO_BUFFER_LENGTH; i++)
	{
		input[i] = 0;
//		output[i] = 0;
	}
	
//	TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);
	input_write_ptr = &input[2];
//	output_read_ptr = &output[0];	
	input_read_ptr = &input[0];
//	period_count=0;
//	output_write_ptr = &output[0];
//	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
/*for(i=0;i<AUDIO_BUFFER_LENGTH;i++)
	{data_word=Sin500[temp++];
	*input_write_ptr++ = (data_word & 0xff00)>>8;
	*input_write_ptr++ = (data_word & 0xff);
	if(temp>=Tx_NUM)
 	    break;
	}*/
	//input_write_ptr = &input[2];

}

/***************************************************************************
Declaration : char read_key(void)

Function :    Reads all keys and returns the key code.
***************************************************************************/
char read_key(void)
{	return(0);
}


