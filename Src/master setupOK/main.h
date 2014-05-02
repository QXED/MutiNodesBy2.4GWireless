/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: main.h
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : 


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version				  
============================================================================*/
#ifndef MAIN_H
#define MAIN_H

/***************************************************************************
* Library Includes
***************************************************************************/

/***************************************************************************
* Local Includes
***************************************************************************/
#include "application.h"

/***************************************************************************
* Constants
***************************************************************************/

// System Parameters
#define FREQ_MIN		0
#define FREQ_MAX		80

// Call States
#define CALL_IDLE		0
#define CALL_SETUP		1
#define CALL_CONNECTED	2
#define CALL_RECONNECT	3

// Call Status
#define MASTER_SYNC		1

// Call Signals
#define SIGNAL_CALL_CLEAR		BIT0

// Audio Buffer
//#define AUDIO_BUFFER_LENGTH	(200)
#define AUDIO_BUFFER_LENGTH	(404)
// Keys
//#define CALL_SETUP_KEY	KBD1
//#define CALL_CLEAR_KEY	KBD3

//Key Codes
#define VOLUME_DOWN		BIT4
#define VOLUME_UP		BIT0
#define MUTE_PLAY		BIT2
#define MUTE_REC		BIT1
#define CALL_CLEARING	BIT3

// EEPROM Addresses
#define EEPROM_ADR_ID0		0
#define EEPROM_ADR_ID1		1
#define EEPROM_ADR_ID2		2
#define EEPROM_ADR_FREQ0	3
#define EEPROM_ADR_FREQ1	4

/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/
extern char call_state;
extern char call_status;
extern char call_timer;
extern char signal_in[];
extern char signal_out[2];

extern char input[AUDIO_BUFFER_LENGTH+4];
//extern char output[AUDIO_BUFFER_LENGTH+4];
volatile extern char *input_write_ptr;
//volatile extern char *output_read_ptr;
extern char *input_read_ptr;
//extern char *output_write_ptr;
extern char freq[2];
extern char setup_freq[4];
//extern unsigned short call_activity_timer;
extern unsigned short frame_loss;
extern short packet_loss[2];

/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/

#endif
