/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: protocol.h
INITIAL AUTHOR........: Nordic Semiconductor 
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : 


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
21.11.2006		1.1			Audio buffer synchronisation flags
06.12.2006		1.2			Test SW includes Modulated Transmitter				  
============================================================================*/
#ifndef PROTOCOL_H
#define PROTOCOL_H

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
extern const char Alaw_compress[2048];
extern const unsigned int Alaw_expand[128];

// Protocol Parameters
#define N_REP_SETUP					1
#define N_REP_RECONNECT				2000
#define N_FREQ_SETUP				4		// Number of freq's in Setup state
#define N_FREQ_CONNECTED			2		// Number of freq's in Connected state
#define N_SYNC_ATTEMPTS				8

// System Definitions
#define SAMPLE_RATE					8		// Sampling frequency in kHz.

// Packet Definitions
#define AUDIO_SAMPLES				30
#define AUDIO_BUFFER_LEVEL			40
#define FRAME_PERIOD				(F_CPU/SAMPLE_RATE*AUDIO_SAMPLES - 1) // 15359

// Status Codes
#define STATUS_RF_READY				0
#define STATUS_TIMEOUT				1
#define STATUS_SYNC_OK				2
#define STATUS_SYNC_FAILED			3
#define STATUS_RX_OK				4
#define STATUS_RX_FAILED			5

// Flags
#define FLAG_RXBUFFER_SYNC			BIT0
#define FLAG_MASTER_SYNC			BIT1
#define FLAG_BUFFER_SYNC1			BIT2
#define FLAG_BUFFER_SYNC2			BIT3

// Event Times
#define MASTER_START_TX0			(SLAVE_START_RX0 + (F_CPU/1000*(50))) // 205
#define MASTER_END_RX0				(MASTER_START_TX0 + (F_CPU/1000*(278+125+50+278+50))) // 3404
#define MASTER_START_TX1			(MASTER_END_RX0 + (F_CPU/1000*(103+50))) // 4031
#define MASTER_END_RX1				(MASTER_START_TX1 + (MASTER_END_RX0-MASTER_START_TX0)) // 7230

#define SLAVE_START_RX0				0
#define SLAVE_END_RX0				(SLAVE_RX0_IRQ + (F_CPU/1000*(50))) // 1548
#define SLAVE_START_TX0				(SLAVE_END_RX0 + (F_CPU/1000*(103+50))) // 2175
#define SLAVE_START_RX1				(SLAVE_START_TX0 + (F_CPU/1000*(278+125))) // 3826
#define SLAVE_END_RX1				(SLAVE_RX1_IRQ + (F_CPU/1000*(50))) // 5375
#define SLAVE_START_TX1				(SLAVE_END_RX1 + (F_CPU/1000*(103+50))) // 6002

// Master-Slave Synchronization
#define SLAVE_RX0_IRQ				(MASTER_START_TX0 + (F_CPU/1000*(278))) // 1344
#define SLAVE_RX1_IRQ				(MASTER_START_TX1 + (F_CPU/1000*(278))) // 5170
#define TX_RX_DELAY					(F_CPU/1000*(49)) // 200 Tuning parameter (must be measured)
#define SYNC_DELAY					-(F_CPU/1000*(24)) //-100 Tuning parameter (must be measured)

// Timeouts
#define TIMEOUT_CALL_SETUP_PACKET	(F_CPU/1000*(166+77)/8) // 124 DIV8
#define TIMEOUT_CALL_SETUP_PACKETS	(F_CPU/1000*(2500)/64) // 160 DIV64
#define TIMEOUT_AUDIO_PACKET		(F_CPU/1000*(293)/8) // 150 DIV8
#define TIMEOUT_SYNC				255 //(F_CPU/1000*(3984)/64) // 255 DIV64
#define TIMEOUT_CALL_CLEAR_MASTER	25	// Number of frames
#define TIMEOUT_CALL_CLEAR_SLAVE	5	// Number of frames
#define TIMEOUT_FRAME_LOSS			250
#define TIMEOUT_CALL_ACTIVITY		300

// Call Status Codes
#define CALL_NO_ACTIVITY			0xF0
#define CALL_ACTIVITY				0xF1
#define CALL_SETUP_OK				0xF2
#define CALL_SETUP_FAILURE			0xF3
#define CALL_CLEAR					0xF4

// Call Signal Codes
#define SIGNAL_CALL_SETUP			0x7D
#define SIGNAL_CALL_SETUP_ACK0		0x7D
#define SIGNAL_CALL_SETUP_ACK1		0x13
#define SIGNAL_PACKET0_OK			BIT0
#define SIGNAL_PACKET1_OK			BIT1

// Frequency Definitions
#define FREQ0_MIN					2
#define FREQ0_MAX					41
#define FREQ1_MIN					42
#define FREQ1_MAX					81
#define FREQ_SETUP0					10
#define FREQ_SETUP1					30
#define FREQ_SETUP2					50
#define FREQ_SETUP3					70
#define FREQ_STEP					10

// Frequency Hopping Parameters
#ifndef TEST_NO_FREQ_HOP
	#define PACKET_LOSS_INCREMENT		2
	#define PACKET_LOSS_DECREMENT		1
#else
	#define PACKET_LOSS_INCREMENT		0
	#define PACKET_LOSS_DECREMENT		0
#endif
#define PACKET_LOSS_THRESHOLD		5000


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
void init_protocol(void);
void init_freq(void);
char call_setup(char *freq_ptr,char n_freqs);
char call_detect(char *freq_ptr,char n_freqs,unsigned int n_rep);
void audio_transfer(void);
char get_sync(void);
#ifdef TEST_TX_CW
	void test_rf_transmitter(char rf_channel);
#endif
#ifdef TEST_TX_MOD
	void test_rf_modulator(char rf_channel);
#endif
#ifdef TEST_RX
	void test_rf_receiver(char rf_channel);
#endif

#endif
