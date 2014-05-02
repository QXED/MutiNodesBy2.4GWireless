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
extern char Alaw_compress[2048];
extern char connectstatus;	

// Protocol Parameters
#define N_REP_SETUP					1
#define N_REP_RECONNECT				2000
#define N_FREQ_SETUP				3		// Number of freq's in Setup state
#define N_FREQ_CONNECTED			2		// Number of freq's in Connected state
#define N_SYNC_ATTEMPTS				8

#define SIGNALING_LEN				4


// Status Codes
#define STATUS_RF_READY				0
#define STATUS_TIMEOUT				1
#define STATUS_SYNC_OK				2
#define STATUS_SYNC_FAILED			3
#define STATUS_RX_OK				4
#define STATUS_RX_FAILED			5

// Flags
#define FLAG_RXBUFFER_SYNC		BIT0
#define FLAG_MASTER_SYNC			BIT1
#define FLAG_BUFFER_SYNC1			BIT2
#define FLAG_BUFFER_SYNC2			BIT3

#define us_125						139
#define us_103						89

// Event Times
#define MASTER_START_TX0			(SLAVE_START_RX0 + (F_TIM3/1000*(50))) //0x258
#define MASTER_END_RX0				(MASTER_START_TX0 + (F_TIM3/1000*(278+us_125+50+278+50))) // 0x26f4
#define MASTER_START_TX1			(MASTER_END_RX0 + (F_TIM3/1000*(us_103+50))) // 0x29b3
#define MASTER_END_RX1				(MASTER_START_TX1 + (MASTER_END_RX0-MASTER_START_TX0)) // 7230

#define SLAVE_START_RX0				0
#define SLAVE_END_RX0				(SLAVE_RX0_IRQ + (F_TIM3/1000*(50))) // 1548
#define SLAVE_START_TX0				(SLAVE_END_RX0 + (F_TIM3/1000*(us_103+50))) // 2175
#define SLAVE_START_RX1				(SLAVE_START_TX0 + (F_TIM3/1000*(278+us_125))) // 3826
#define SLAVE_END_RX1				(SLAVE_RX1_IRQ + (F_TIM3/1000*(50))) // 5375
#define SLAVE_START_TX1				(SLAVE_END_RX1 + (F_TIM3/1000*(us_103+50))) // 6002

// Master-Slave Synchronization
#define SLAVE_RX0_IRQ				(MASTER_START_TX0 + (F_TIM3/1000*(278))) // 1344
#define SLAVE_RX1_IRQ				(MASTER_START_TX1 + (F_TIM3/1000*(278))) // 5170
#define TX_RX_DELAY					(F_TIM3/1000*(30)) // 200 Tuning parameter (must be measured) 49
#define SYNC_DELAY					-(F_TIM3/1000*(0)) //-100 Tuning parameter (must be measured) 24 

// Timeouts
#define TIMEOUT_CALL_SETUP_PACKET	(F_TIM4/1000*(166+77)) // 124 DIV8
#define TIMEOUT_CALL_SETUP_PACKETS	(F_TIM4/1000*(2500)) // 160 DIV64
#define TIMEOUT_AUDIO_PACKET		(F_TIM4/1000*(293)) // 150 DIV8
#define TIMEOUT_SYNC				(F_TIM4/1000*(3984)) // 255 DIV64
#define TIMEOUT_CALL_CLEAR_MASTER	25	// Number of frames
#define TIMEOUT_CALL_CLEAR_SLAVE	5	// Number of frames
#define TIMEOUT_FRAME_LOSS			250
#define TIMEOUT_CALL_ACTIVITY		300

#define TIMEOUT_AUDIO_PACKET_ESB		(F_TIM4/1000*(3000)) // 150 DIV8


#define TIMEOUT_TEST_RX    			(F_TIM4/1000*(5000))

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
#define FREQ_SETUP0					15//0X0a
#define FREQ_SETUP1					25//0X1E
#define FREQ_SETUP2					35//0X32
#define FREQ_SETUP3					45//0X46
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

#define CLIENTID_NULL					0x00
#define IS_CLIENTID_NULL(nBYTE)	(CLIENTID_NULL==(nBYTE))

#define CTRL_PACKET_LOSS_LIMIT              20


#define PERIOD_NUM	10
#define PERIOD_CTRL	9
#define PERIOD_INVALID	-1
#define PERIOD_1ST_AUDIO	0
#define IS_PERIOD_CTRL(nWORD)	(PERIOD_CTRL==(nWORD))
#define IS_PERIOD_INVALID(nWORD)	(PERIOD_INVALID==(nWORD))
#define IS_PERIOD_AUDIO(nWORD)		(PERIOD_INVALID<(nWORD) && PERIOD_CTRL>(nWORD))
#define IS_PERIOD_1ST_AUDIO(nWORD)	(PERIOD_1ST_AUDIO==(nWORD))


#define GET_CLIENT_ID(nBYTE)	((nBYTE) & ~BIT7)

#define BC_AUDIO_CLIENT_ID		((0x55) & ~BIT7)

#define BC_CTRL_CLIENT_ID		((0x55) | BIT7)

#define BC_ADDR_ID	4
#define IS_BC_ADDR_ID(nBYTE)	(BC_ADDR_ID==nBYTE)
#define IS_NOT_BC_ADDR_ID(nBYTE)	(BC_ADDR_ID!=nBYTE)

#define TOTAL_ADDR_NUM	6
#define TX_ADDR_NUM		5
#define CLIENT_NUM			4

#define ADDRESS_LENGTH	3

#define SETUP_TX_ADDR_NUM	TX_ADDR_NUM-1

/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/

extern char rf_address[TOTAL_ADDR_NUM][ADDRESS_LENGTH];
extern char client_id[];
extern char server_id;
/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/
void init_protocol(void);
void init_freq(void);
char call_setup(char *freq_ptr,char n_freqs);
bool audio_transfer(void);
void data_transfer(void);
void put_contral_packet(void);
void put_setup_packet(char addr_byte);
void init_ctrl(void);
void get_ack_packet(char addr_byte);
bool get_setup_packet(char addr_id);
void put_audio_packet(void);
void ctrl_transfer(void);
#endif
