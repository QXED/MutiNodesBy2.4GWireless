/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: main.c
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : Main application handling MMI and Call control.


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
06.11.2006		1.1			USB Dongle don't stop audio interface when reconnect
08.11.2006		1.2			USB Suspend Mode power management
06.12.2006		1.3			Test SW includes Modulated Transmitter 				  
============================================================================*/


/****************************************************************************
* Library Includes
*****************************************************************************/
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

/****************************************************************************
* Local Includes
****************************************************************************/
#include "main.h"
#include "protocol.h"
#include "audio.h"
#include "mcu.h"
#include "radio.h"

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
char signal_in[2];
char signal_out[2];
char key_code;
char input[AUDIO_BUFFER_LENGTH+4];
char output[AUDIO_BUFFER_LENGTH+4];
volatile char *input_write_ptr;
volatile char *output_read_ptr;
char *input_read_ptr;
char *output_write_ptr;
char freq[2];
char setup_freq[4];
unsigned int call_activity_timer;
unsigned int frame_loss;
int packet_loss[2];

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
	init_rf();
	init_buffer();
	init_protocol();
	init_freq();
	
	#ifdef TEST_TX_CW
		test_rf_transmitter(78);
	#endif
	#ifdef TEST_TX_MOD
		test_rf_modulator(81);
	#endif
	#ifdef TEST_RX
		test_rf_receiver(78);
	#endif
		
	/* Main Background loop */
	call_state = CALL_IDLE;
	
	while(1)
	{
		/* Call States */	
		switch (call_state)
		{
			case CALL_IDLE:
				#ifdef DONGLE
					sleep(WDT_TIMEOUT_60MS,STANDBY_MODE);
					call_status = CALL_NO_ACTIVITY;
					#ifdef USB
						SET_VOLUME_DOWN;
						SET_VOLUME_UP;
						SET_MUTE_PLAY;
						SET_MUTE_REC;
						if(CALL_ACTIVITY_PIN)
							call_status = CALL_ACTIVITY;
					#else
						if(!CALL_SETUP_KEY)
							call_status = CALL_ACTIVITY;
					#endif
					if(call_status == CALL_ACTIVITY)
						call_state = CALL_SETUP;
				#endif
				
				#ifdef HEADSET
					sleep(WDT_TIMEOUT_1S,POWER_DOWN_MODE);
					call_state = CALL_SETUP;
				#endif
				
			break;
			
			case CALL_SETUP:
				#ifdef DONGLE
					LED_ON;
					call_status = call_setup(&setup_freq[0],N_FREQ_SETUP);
					LED_OFF;
					if(call_status != CALL_SETUP_FAILURE)
					{
						init_buffer();
						init_rf();
						init_protocol();
						init_codec();
						start_codec();
						#ifdef USB
							// Enable watchdog to handle USB Suspend Mode
							wdt_enable(WDT_TIMEOUT_15MS);
						#else
							start_timer1(0,FRAME_PERIOD, DIV1);
						#endif
						call_state = CALL_CONNECTED;
					}	
					else
						call_state = CALL_IDLE;
				#endif
								
				#ifdef HEADSET
					LED_ON;
					call_status = call_detect(&setup_freq[0],N_FREQ_SETUP,N_REP_SETUP);
					LED_OFF;
					if(call_status != CALL_SETUP_FAILURE)
					{
						init_buffer();
						init_rf();
						init_protocol();
						init_codec();
						call_status &= ~MASTER_SYNC;
						start_timer1(0,FRAME_PERIOD, DIV1);
						call_state = CALL_CONNECTED;
					}
					else
						call_state = CALL_IDLE;
				#endif
			break;
			
			case CALL_CONNECTED:
				#ifdef DONGLE
					while(1)
					{
						// USB Dongle clears watchdog handling USB Suspend Mode
						#ifdef USB
							wdt_reset();
						#endif
						
						// Send and receive audio packet
						audio_transfer();
						
						// Handle key code from HEADSET
						key_code = (signal_in[1] & 0x1F);
						if(key_code != 0)
							LED_ON;
						else
							LED_OFF;
							
						#ifdef USB
							if(key_code & VOLUME_DOWN)
								CLEAR_VOLUME_DOWN;
							else
								SET_VOLUME_DOWN;
								
							if(key_code & VOLUME_UP)
								CLEAR_VOLUME_UP;
							else
								SET_VOLUME_UP;
								
							if(key_code & MUTE_PLAY)
								CLEAR_MUTE_PLAY;
							else
								SET_MUTE_PLAY;
								
							if(key_code & MUTE_REC)
								CLEAR_MUTE_REC;
							else
								SET_MUTE_REC;
						#endif
						
						// Check if call is to be cleared	
						#ifdef USB
							if(!CALL_ACTIVITY_PIN)
							{
								call_activity_timer += 1;
								if(call_activity_timer >= TIMEOUT_CALL_ACTIVITY)
									call_status = CALL_CLEAR;
							}
							else
								call_activity_timer = 0;
						
						#else
							if(!CALL_CLEAR_KEY)
								call_status = CALL_CLEAR;
						#endif
						
						
							
						// Call clearing by HEADSET or DONGLE
						if((key_code == CALL_CLEARING) || (call_status == CALL_CLEAR))
						{
							signal_out[0] |= SIGNAL_CALL_CLEAR;
							call_timer += 1;
							if(call_timer >= TIMEOUT_CALL_CLEAR_MASTER)
							{
								call_state = CALL_IDLE;
								stop_codec();
								init_buffer();
								init_rf();
								init_protocol();
								init_codec();
								eeprom_write(freq[0],EEPROM_ADR_FREQ0);
								eeprom_write(freq[1],EEPROM_ADR_FREQ1);
								LED_OFF;
								#ifdef USB
									// Disable watchdog used to handle USB Suspend Mode
									wdt_disable();
								#endif
								break;
							}
						}
						else
							signal_out[0] &= ~SIGNAL_CALL_CLEAR;
	
						// Call clearing due to Frame Loss
						if(frame_loss >= TIMEOUT_FRAME_LOSS)
						{
							#ifdef USB
								call_state = CALL_RECONNECT;
								init_rf();
								init_protocol();
								// Disable watchdog used to handle USB Suspend Mode
								wdt_disable();
							#else
								call_state = CALL_RECONNECT;
								stop_codec();
								init_buffer();
								init_rf();
								init_protocol();
								init_codec();
							#endif
							break;
						}
					}
				#endif
				
				#ifdef HEADSET
					while(1)
					{
						if(call_status & MASTER_SYNC)
						{
							audio_transfer();
						}
						else
						{
							call_status = get_sync();
							if(call_status & MASTER_SYNC)
								start_codec();
							else
								frame_loss += 10;
						}
						
						// Read and handle keys
						key_code = read_key();
						signal_out[1] &= 0xE0;
						signal_out[1] |= key_code;
						
						
						// Call cleared by DONGLE
						if(signal_in[0] & SIGNAL_CALL_CLEAR)
						{
							call_timer += 1;
							if(call_timer >= TIMEOUT_CALL_CLEAR_SLAVE)
							{
								call_state = CALL_IDLE;
								stop_codec();
								init_buffer();
								init_rf();
								init_protocol();
								init_codec();
								break;
							}
						}
						else
							call_timer = 0;
						
						// Call clearing due to Frame Loss
						if(frame_loss >= TIMEOUT_FRAME_LOSS)
						{
							call_state = CALL_RECONNECT;
							stop_codec();
							init_buffer();
							init_rf();
							init_protocol();
							init_codec();
							break;
						}
					}
				#endif
			break;

			case CALL_RECONNECT:
				#ifdef DONGLE
					LED_ON;
					call_status = call_setup(&setup_freq[0],N_FREQ_SETUP);
					LED_OFF;
					if(call_status != CALL_SETUP_FAILURE)
					{
						#ifdef USB
							init_rf();
							init_protocol();
							reset_codec();
							call_state = CALL_CONNECTED;
						#else
							init_buffer();
							init_rf();
							init_protocol();
							init_codec();
							start_codec();
							start_timer1(0,FRAME_PERIOD, DIV1);
							call_state = CALL_CONNECTED;
						#endif
					}	
					else
					{
						stop_codec();
						init_buffer();
						init_rf();
						init_protocol();
						init_codec();
						call_state = CALL_IDLE;
					}
				#endif
				
				#ifdef HEADSET
					LED_ON;
					call_status = call_detect(&setup_freq[0],N_FREQ_SETUP,N_REP_RECONNECT);
					LED_OFF;
					if(call_status != CALL_SETUP_FAILURE)
					{
						init_buffer();
						init_rf();
						init_protocol();
						init_codec();
						call_status &= ~MASTER_SYNC;
						start_timer1(0,FRAME_PERIOD, DIV1);
						call_state = CALL_CONNECTED;
					}
					else
						call_state = CALL_IDLE;

				#endif
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
	for(i=0; i<AUDIO_BUFFER_LENGTH; i++)
	{
		input[i] = 0;
		output[i] = 0;
	}
	
	cli();
	#ifdef USB
		input_write_ptr = &input[1];
	#else
		input_write_ptr = &input[2];
	#endif
	output_read_ptr = &output[0];	
	input_read_ptr = &input[0];
	output_write_ptr = &output[0];
	sei();
}

/***************************************************************************
Declaration : char read_key(void)

Function :    Reads all keys and returns the key code.
***************************************************************************/
char read_key(void)
{
char key_code = 0;

	if(!KBD0)
		key_code |= BIT0;
	if(!KBD1)
		key_code |= BIT1;
	if(!KBD2)
		key_code |= BIT2;
	if(!KBD3)
		key_code |= BIT3;
	if(!KBD4)
		key_code |= BIT4;
		
	return(key_code);
}


