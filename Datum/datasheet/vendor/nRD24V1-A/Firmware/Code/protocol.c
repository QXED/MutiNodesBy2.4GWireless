/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: protocol.c
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : Functions handling the RF protocol (Master and Slave).


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
21.11.2006		1.1			Audio buffer synchronisation flags
06.12.2006		1.2			Test SW includes Modulated Transmitter				  
============================================================================*/

/***************************************************************************
* Library Includes
***************************************************************************/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

/***************************************************************************
* Local Includes
***************************************************************************/
#include "protocol.h"
#include "radio.h"
#include "main.h"
#include "mcu.h"

/***************************************************************************
* Constants
***************************************************************************/
const unsigned int Alaw_expand[128] PROGMEM=
{
0,16,32,48,64,80,96,112,
128,144,160,176,192,208,224,240,
256,272,288,304,320,336,352,368,
384,400,432,448,464,480,496,528,
544,576,592,624,640,672,704,736,
768,800,832,864,912,944,992,1024,
1072,1120,1168,1216,1280,1328,1392,1456,
1520,1584,1648,1712,1792,1872,1952,2032,
2128,2224,2320,2416,2528,2624,2752,2864,
2992,3120,3248,3392,3552,3696,3856,4032,
4208,4384,4576,4784,4992,5200,5440,5664,
5920,6176,6448,6720,7024,7328,7648,7984,
8320,8688,9072,9472,9872,10304,10768,11232,
11728,12224,12768,13328,13904,14512,15152,15808,
16496,17216,17968,18752,19568,20432,21312,22256,
23216,24240,25296,26400,27552,28752,30016,31328,
};

const char Alaw_compress[2048] PROGMEM=
{
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,25,26,27,28,29,30,
30,31,32,32,33,34,34,35,36,36,37,37,38,38,39,39,
40,40,41,41,42,42,43,43,43,44,44,45,45,45,46,46,
47,47,47,48,48,48,49,49,49,50,50,50,51,51,51,51,
52,52,52,53,53,53,53,54,54,54,54,55,55,55,55,56,
56,56,56,57,57,57,57,58,58,58,58,59,59,59,59,59,
60,60,60,60,60,61,61,61,61,61,62,62,62,62,62,63,
63,63,63,63,63,64,64,64,64,64,64,65,65,65,65,65,
65,66,66,66,66,66,66,67,67,67,67,67,67,67,68,68,
68,68,68,68,69,69,69,69,69,69,69,69,70,70,70,70,
70,70,70,71,71,71,71,71,71,71,71,72,72,72,72,72,
72,72,72,73,73,73,73,73,73,73,73,74,74,74,74,74,
74,74,74,74,75,75,75,75,75,75,75,75,75,75,76,76,
76,76,76,76,76,76,76,77,77,77,77,77,77,77,77,77,
77,78,78,78,78,78,78,78,78,78,78,78,79,79,79,79,
79,79,79,79,79,79,79,80,80,80,80,80,80,80,80,80,
80,80,81,81,81,81,81,81,81,81,81,81,81,81,82,82,
82,82,82,82,82,82,82,82,82,82,82,83,83,83,83,83,
83,83,83,83,83,83,83,83,84,84,84,84,84,84,84,84,
84,84,84,84,84,85,85,85,85,85,85,85,85,85,85,85,
85,85,85,85,86,86,86,86,86,86,86,86,86,86,86,86,
86,86,87,87,87,87,87,87,87,87,87,87,87,87,87,87,
87,87,88,88,88,88,88,88,88,88,88,88,88,88,88,88,
88,88,89,89,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,91,91,91,91,91,91,91,91,91,91,91,91,
91,91,91,91,91,91,91,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,94,
94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,95,95,95,95,95,95,95,95,95,95,95,95,95,
95,95,95,95,95,95,95,95,96,96,96,96,96,96,96,96,
96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,
99,99,99,99,99,99,99,99,99,100,100,100,100,100,100,100,
100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
100,100,100,100,101,101,101,101,101,101,101,101,101,101,101,101,
101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,
101,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,
102,102,102,102,102,102,102,102,102,102,102,102,102,102,103,103,
103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,
103,103,103,103,103,103,103,103,103,103,103,103,103,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,
110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,
110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,
111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,
111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,
111,111,111,111,111,111,111,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,114,114,114,114,114,114,114,114,114,114,114,114,114,
114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,
114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,
114,114,114,114,115,115,115,115,115,115,115,115,115,115,115,115,
115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,
115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,
115,115,115,115,115,115,115,116,116,116,116,116,116,116,116,116,
116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,
116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,
116,116,116,116,116,116,116,116,116,116,116,116,116,117,117,117,
117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
117,117,117,117,118,118,118,118,118,118,118,118,118,118,118,118,
118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,
118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,
118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,119,
119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,
119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,
119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,
119,119,119,119,119,119,119,119,119,119,119,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,120,121,121,121,121,121,
121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,121,121,121,121,121,121,121,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
};

/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/
char protocol_flags = 0;
static unsigned int event_times[6];
static unsigned int master_sync_times[2];
unsigned int *event_time;
//static int packet_loss[2];

/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/
void put_audio_packet(void);
void get_audio_packet(void);
void stuff_packet(void);
char compress_audio(int audio_linear16);
int expand_audio(char audio_compressed8);

/***************************************************************************
Declaration : char call_setup(char *freq_list,char n_freqs,unsigned int n_rep)

Description : A 'Call_Setup' packet is transmitted at the given frequencies. 
			  After each transmission it waits for a 'Call_Setup_Ack' packet
			  to be received. If an acknowledge is received it returns the
			  frequency used or else it returns CALL_SETUP_FAILURE when
			  there is no longer call activity.
***************************************************************************/
char call_setup(char *freq_list,char n_freqs)
{
char i;
char *freq_ptr;
char status;
char payload[3];
	
	// Set Payload = 'Call_Setup' packet
	init_rf_signalling();
	payload[0] = SIGNAL_CALL_SETUP;
	payload[1] = freq[0];
	payload[2] = freq[1];
	write_rf_data(W_TX_PAYLOAD,&payload[0],3);
	
	// Re-use payload for all following transmissions
	reuse_rf_payload();

	// Repeat frequency sequence n_rep times
	call_status = CALL_ACTIVITY;
	while(call_status == CALL_ACTIVITY)
	{
		freq_ptr = freq_list;
		
		// Try to set up call at each frequency
		for(i=0; i<n_freqs; i++)
		{
			// Send 'Call_Setup' packet
			set_rf_tx(*freq_ptr);
			clear_rf_interrupt();
			rf_enable_pulse();
			status = wait_rf_irq(TIMEOUT_CALL_SETUP_PACKET,DIV8);
			if(status == STATUS_TIMEOUT)
				return(CALL_SETUP_FAILURE);
			
			// Wait for 'Call_Setup_Ack' to be received
			set_rf_rx(*freq_ptr);
			clear_rf_interrupt();
			RF_ENABLE;
			status = wait_rf_irq(TIMEOUT_CALL_SETUP_PACKET,DIV8);
			RF_DISABLE;

			// If 'Call_Setup_Ack' is received return with RF channel
			if (status != STATUS_TIMEOUT)
			{
				// check if valid Call Setup Ack Packet
				ENABLE_RF_SPI;
				read_rf_byte(R_RX_PAYLOAD);
				payload[0] = read_rf_byte(R_RX_PAYLOAD);
				payload[1] = read_rf_byte(R_RX_PAYLOAD);
				DISABLE_RF_SPI;
				if((payload[0] == SIGNAL_CALL_SETUP_ACK0) && (payload[1] == SIGNAL_CALL_SETUP_ACK1))
					return(*freq_ptr);
				else
					freq_ptr++;
			}		
			// else try next frequency
			else
				freq_ptr++;
		}
		
		#ifdef USB
		if(!CALL_ACTIVITY_PIN)
		{
			call_activity_timer += 1;
			if(call_activity_timer >= TIMEOUT_CALL_ACTIVITY)
				call_status = CALL_NO_ACTIVITY;
		}
		else
			call_activity_timer = 0;
		
		// Abort if USB Suspend Mode
		if(USB_SUSPEND)
			return(CALL_SETUP_FAILURE);
		
		#else
		if(!CALL_CLEAR_KEY)
			call_status = CALL_NO_ACTIVITY;
		#endif
	}

	// No 'Call_Setup_Ack' is received
	return(CALL_SETUP_FAILURE);
}

/***************************************************************************
Declaration : char call_detect(char *freq_list,char n_freqs,unsigned int n_rep)

Description : A 'Call_Setup'is searched for at the given frequencies. 
			  If a 'Call_Setup' is received, a 'Call_Setup_Ack' is transmitted
			  and the frequency used is returned.
			  If no 'Call_Setup'packet is received during the repetitions
			  given by n_rep, it returns CALL_SETUP_FAILURE.
***************************************************************************/
char call_detect(char *freq_list,char n_freqs,unsigned int n_rep)
{
unsigned int i,j;
char *freq_ptr;
char status;
char payload[3];

	// Set Payload = 'Call_Setup_Ack' packet
	init_rf_signalling();
	payload[0] = SIGNAL_CALL_SETUP_ACK0;
	payload[1] = SIGNAL_CALL_SETUP_ACK1;
	payload[2] = 0;
	write_rf_data(W_TX_PAYLOAD,&payload[0],3);
	reuse_rf_payload();
	
	// Repeat frequency sequence n_rep times
	for(i=0; i<n_rep; i++)
	{
		freq_ptr = freq_list;
		
		// Try to set up call at each frequency
		for(j=0; j<n_freqs; j++)
		{
			// Wait for 'Call_Setup' to be received
			set_rf_rx(*freq_ptr);
			clear_rf_interrupt();
			RF_ENABLE;
			status = wait_rf_irq(TIMEOUT_CALL_SETUP_PACKETS,DIV64);
			RF_DISABLE;
			
			// If 'Call_Setup' is received return with RF channel
			if (status != STATUS_TIMEOUT)
			{
				// Send 'Call_Setup_Ack' packet and return RF Channel
				set_rf_tx(*freq_ptr);
				clear_rf_interrupt();
				rf_enable_pulse();
				status = wait_rf_irq(TIMEOUT_CALL_SETUP_PACKET,DIV8);
				if(status != STATUS_TIMEOUT)
				{
					// check if valid Call Setup Packet
					ENABLE_RF_SPI;
					read_rf_byte(R_RX_PAYLOAD);
					payload[0] = read_rf_byte(R_RX_PAYLOAD);
					payload[1] = read_rf_byte(R_RX_PAYLOAD);
					payload[2] = read_rf_byte(R_RX_PAYLOAD);
					DISABLE_RF_SPI;
					if(payload[0] == SIGNAL_CALL_SETUP)
					{
						freq[0] = payload[1];
						freq[1] = payload[2];
						return(CALL_SETUP_OK);
					}
					else
						freq_ptr++;
				}
				else
					return(CALL_SETUP_FAILURE);
			}	
			// else try next frequency
			else
				freq_ptr++;
		}
	}
	
	// No 'Call_Setup_Ack' is received
	return(CALL_SETUP_FAILURE);
}

/***************************************************************************
Declaration : void audio_transfer(void)

Description : Writes an audio payload to the radio and exchanges audio
			  payloads at the given frequencies. Reads the audio payload
			  from the radio if any is received.
***************************************************************************/
void audio_transfer(void)
{
char status[2];
int i;

	#ifdef DONGLE
	
	// Exchange Audio Packets at each frequency
	event_time = &event_times[0];
	for(i=0; i<N_FREQ_CONNECTED; i++)
	{
		// Send TX packet synchronized to frame
		set_rf_tx(freq[i]);
		clear_rf_interrupt();
		#ifdef USB
			SET_TIMER2_EVENT((char)*event_time++);
			while(!TIMER2_EVENT)
				;
		#else
			SET_TIMER1_EVENT(*event_time++);
			while(!TIMER1_EVENT)
				;
		#endif
		rf_enable_pulse();
		wait_rf_irq(TIMEOUT_AUDIO_PACKET,DIV8);
		
		// Receive Audio RX packet
		set_rf_rx(freq[i]);
		clear_rf_interrupt();
		RF_ENABLE;
		#ifdef USB
			SET_TIMER2_EVENT((char)*event_time++);
			while(!TIMER2_EVENT)
				;
		#else
			SET_TIMER1_EVENT(*event_time++);
			while(!TIMER1_EVENT)
				;
		#endif
		if(!nRF_IRQ)
			status[i] = STATUS_RX_OK;
		else
			status[i] = STATUS_RX_FAILED;

		RF_DISABLE;
	}
	
	#ifdef USB
		STOP_TIMER2;
	#endif
	
	// Read RX Audio Packet if any, else stuff a packet
	if((status[0] == STATUS_RX_OK) || (status[1] == STATUS_RX_OK))
	{
		get_audio_packet();
		frame_loss = 0;
	}
	else
	{
		// Set Signal
		stuff_packet();
		frame_loss += 1;
	}
	
	// Flush RX FIFO	
	flush_rf_rx();
	
	
	// Update Packet Loss counters
	if((signal_in[0] & SIGNAL_PACKET0_OK) && (status[0] == STATUS_RX_OK))
	{
		packet_loss[0] -= PACKET_LOSS_DECREMENT;
		if(packet_loss[0] < 0)
			packet_loss[0] = 0;
	}
	else
		packet_loss[0] += PACKET_LOSS_INCREMENT;
	
	if((signal_in[0] & SIGNAL_PACKET1_OK) && (status[1] == STATUS_RX_OK))
	{
		packet_loss[1] -= PACKET_LOSS_DECREMENT;
		if(packet_loss[1] < 0)
			packet_loss[1] = 0;
	}
	else
		packet_loss[1] += PACKET_LOSS_INCREMENT;
		
	// Change Frequency based on Packet Loss Statistics
	if(packet_loss[0] >= PACKET_LOSS_THRESHOLD)
	{
		freq[0] += FREQ_STEP;
		if(freq[0] > FREQ0_MAX)
			freq[0] -= (FREQ0_MAX - FREQ0_MIN);
		packet_loss[0] = 0;
		packet_loss[1] = 0;
	}
	if(packet_loss[1] >= PACKET_LOSS_THRESHOLD)
	{
		freq[1] += FREQ_STEP;
		if(freq[1] > FREQ1_MAX)
			freq[1] -= (FREQ1_MAX - FREQ1_MIN);
		packet_loss[0] = 0;
		packet_loss[1] = 0;
	}
	
	signal_out[1] = freq[1] - freq[0];

	//Write TX Audio Packet and enable payload re-use
	put_audio_packet();
	reuse_rf_payload();
	
	#endif
	
	#ifdef HEADSET
	
	// Exchange Audio Packets at each frequency
	event_time = &event_times[0];
	SET_TIMER1_EVENT(*event_time++);
	for(i=0; i<N_FREQ_CONNECTED; i++)
	{
		// Receive Audio RX packet
		set_rf_rx(freq[i]);
		clear_rf_interrupt();
		while(!TIMER1_EVENT)
			;
		RF_ENABLE;
		SET_TIMER1_EVENT(*event_time++);
		while(!TIMER1_EVENT && nRF_IRQ)
		//while(!TIMER1_EVENT)
			;
		// If RF Interrupt request, RX OK
		if(!nRF_IRQ)
		{
			status[i] = STATUS_RX_OK;
			SET_TIMER1(master_sync_times[i]);
		}
		else
			status[i] = STATUS_RX_FAILED;

		RF_DISABLE;
		
		// Send Audio TX packet
		set_rf_tx(freq[i]);
		clear_rf_interrupt();
		SET_TIMER1_EVENT(*event_time++);
		while(!TIMER1_EVENT)
			;
		rf_enable_pulse();
		SET_TIMER1_EVENT(*event_time++);
		wait_rf_irq(TIMEOUT_AUDIO_PACKET,DIV8);
	}

	// Read RX Audio Packet if any, else stuff a packet
	if((status[0] == STATUS_RX_OK) || (status[1] == STATUS_RX_OK))
	{
		get_audio_packet();
		frame_loss = 0;
	}
	else
	{
		stuff_packet();
		signal_in[0] &= ~SIGNAL_CALL_CLEAR;
		frame_loss += 1;
	}

	// Flush RX FIFO	
	flush_rf_rx();
	
	// Set Receive Status bits and update Frequency
	if(status[0] == STATUS_RX_OK)
	{
		signal_out[0] |= SIGNAL_PACKET0_OK;
		if(status[1] == STATUS_RX_OK)
			signal_out[0] |= SIGNAL_PACKET1_OK;
		else
		{
			signal_out[0] &= ~SIGNAL_PACKET1_OK;
			freq[1] = freq[0] + signal_in[1];
		}
	}
	else
	{
		signal_out[0] &= ~SIGNAL_PACKET0_OK;
		if(status[1] == STATUS_RX_OK)
		{
			signal_out[0] |= SIGNAL_PACKET1_OK;
			freq[0] = freq[1] - signal_in[1];
		}
		else
			signal_out[0] &= ~SIGNAL_PACKET1_OK;
	}
	
	// Write TX Audio Packet and enable payload re-use
	put_audio_packet();
	reuse_rf_payload();
	
	#endif
}

/***************************************************************************
Declaration : char get_sync(void)

Description : The Slave synchronizes to the Master frame by resetting the 
			  frame timer when a valid packet is received. 
***************************************************************************/
char get_sync(void)
{
char status;
char sync_counter = 0;
int i;

	// Receive until valid packet or timeout
	while(1)
	{
		for(i=0; i<N_FREQ_CONNECTED; i++)
		{
			set_rf_rx(freq[i]);
			clear_rf_interrupt();
			RF_ENABLE;
			status = wait_rf_irq(TIMEOUT_SYNC,DIV64);
			RF_DISABLE;
			if(status == STATUS_TIMEOUT)
			{
				// Increment sync attempt counter
				sync_counter += 1;
				if(sync_counter >= N_SYNC_ATTEMPTS)
					return(call_status & ~MASTER_SYNC);
			}
			else
			{
				SET_TIMER1((master_sync_times[i]+SYNC_DELAY));
				return(call_status | MASTER_SYNC);
			}
		}
	}
}

/***************************************************************************
Declaration : void put_audio_packet(void)

Description : 
***************************************************************************/
void put_audio_packet(void)
{
int audio_sample,abs_audio;
volatile char status;
char audio_byte;
int i;
	
	// Put RF Payload
	ENABLE_RF_SPI;
	write_rf_byte(W_TX_PAYLOAD);
	
	// Put Signalling bytes
	write_rf_byte(signal_out[0]);
	write_rf_byte(signal_out[1]);
	
	// Put audio bytes
	for(i=0; i< AUDIO_SAMPLES; i++)
	{
		// Read 16bit audio sample from input buffer
		audio_sample = (unsigned int)(*input_read_ptr++) << 8;
		audio_sample += (unsigned int)*input_read_ptr++;
		#ifndef USB
		input_read_ptr += 2;
		#endif
		if(input_read_ptr >= &input[AUDIO_BUFFER_LENGTH])
			input_read_ptr = &input[0];
		
		// Compress 16bit --> 12bit --> 8bit
		if(audio_sample < 0)
			abs_audio = -audio_sample;
		else
			abs_audio = audio_sample;
		audio_byte = pgm_read_byte(&Alaw_compress[(abs_audio >> 4) & 0x7FF]);
		if(audio_sample < 0)
			audio_byte |= 0x80;
		
		// Put audio byte in nRF24L01 FIFO
		READ_SPI_STATUS(status);
		WRITE_SPI(audio_byte);
	}
	
	if(!(protocol_flags & FLAG_BUFFER_SYNC1))
	{
		input_read_ptr = (char *)((unsigned int)input_write_ptr & 0xFFFC);
		input_read_ptr -= 40;
		if(input_read_ptr < &input[0])
			input_read_ptr += AUDIO_BUFFER_LENGTH;
		protocol_flags |= FLAG_BUFFER_SYNC1;
	}
	
	#ifdef HEADSET
	// Handle slip due to clock differences
	i = (int)input_write_ptr - (int)input_read_ptr;
	if(i < 0)
		i += AUDIO_BUFFER_LENGTH;
	if(i < 20)
	{
		#ifdef USB
		input_read_ptr -= 2;
		#else
		input_read_ptr -= 4;
		#endif
		if(input_read_ptr < &input[0])
			input_read_ptr = &input[AUDIO_BUFFER_LENGTH-1];
	}
	if(i > 60)
	{
		#ifdef USB
		input_read_ptr -= 2;
		#else
		input_read_ptr -= 4;
		#endif
		if(input_read_ptr >= &input[AUDIO_BUFFER_LENGTH])
			input_read_ptr = &input[0];
	}
	#endif

	WAIT_SPI_READY;
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void get_audio_packet(void)

Description : 
***************************************************************************/
void get_audio_packet(void)
{
int audio_sample;
volatile char status;
char audio_byte;
int i;

	ENABLE_RF_SPI;
	read_rf_byte(R_RX_PAYLOAD);
	signal_in[0] = read_rf_byte(R_RX_PAYLOAD);
	signal_in[1] = read_rf_byte(R_RX_PAYLOAD);
	WRITE_SPI(R_RX_PAYLOAD);
	WAIT_SPI_READY;

	for(i=0; i< AUDIO_SAMPLES; i++)
	{
		// Get audio byte in nRF24L01 FIFO
		READ_SPI(audio_byte);
		WRITE_SPI(R_RX_PAYLOAD);
		
		// Expand audio to 16-bit
		audio_sample = expand_audio(audio_byte);
		
		*output_write_ptr++ = (char)(audio_sample >> 8);
		*output_write_ptr++ = (char)(audio_sample);
		#ifndef USB
		*output_write_ptr++ = 0;
		*output_write_ptr++ = 0;
		#endif
		if(output_write_ptr >= &output[AUDIO_BUFFER_LENGTH])
			output_write_ptr = &output[0];
	}
	
	if(!(protocol_flags & FLAG_BUFFER_SYNC2))
	{
		output_write_ptr = (char *)((unsigned int)output_read_ptr + 0xFFFC);
		output_write_ptr -= 40;
		if(output_write_ptr < &output[0])
			output_write_ptr += AUDIO_BUFFER_LENGTH;
		protocol_flags |= FLAG_BUFFER_SYNC2;
	}
	
	#ifdef HEADSET
	// Handle slip due to clock differences
	i = (int)output_read_ptr - (int)output_write_ptr;
	if(i < 0)
		i += AUDIO_BUFFER_LENGTH;
	if(i < 20)
	{
		#ifdef USB
		output_write_ptr -= 2;
		#else
		output_write_ptr -= 4;
		#endif
		if(output_write_ptr < &output[0])
			output_write_ptr = &output[AUDIO_BUFFER_LENGTH-1];
	}
	if(i > 60)
	{
		#ifdef USB
		output_write_ptr -= 2;
		#else
		output_write_ptr -= 4;
		#endif
		if(output_write_ptr >= &output[AUDIO_BUFFER_LENGTH])
			output_write_ptr = &output[0];
	}
	#endif
		
	READ_SPI_STATUS(status);
	READ_SPI(audio_byte);
	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void stuff_packet(void)

Description : 
***************************************************************************/
void stuff_packet(void)
{
int i;
	
	// Zero fill audio packet
	for(i=0; i< AUDIO_SAMPLES; i++)
	{

		*output_write_ptr++ = 0;
		*output_write_ptr++ = 0;
		#ifndef USB
		output_write_ptr += 2;
		#endif
		if(output_write_ptr >= &output[AUDIO_BUFFER_LENGTH])
			output_write_ptr = &output[0];
	}
}

/***************************************************************************
Declaration : char compress_audio(int audio_linear16)

Description : Compresses 16-bit linear to 8-bit using binary search in a
			  128 word A-law compression table.
***************************************************************************/
char compress_audio(int audio_linear16)
{
unsigned int abs_linear16;
unsigned int table_val;
unsigned int index = 64;

	// Get the magnitude
	if(audio_linear16 < 0)
		abs_linear16 = (unsigned int)(-audio_linear16);
	else
		abs_linear16 = (unsigned int)audio_linear16;
		
	// Find compressed value as the index in the compression table using binary search.
	table_val = pgm_read_word(&Alaw_expand[64]);

	if(abs_linear16 < table_val)
		index -= 32;
	else
		index += 32;
	table_val = pgm_read_word(&Alaw_expand[index]);
	
	if(abs_linear16 < table_val)
		index -= 16;
	else
		index += 16;
	table_val = pgm_read_word(&Alaw_expand[index]);

	if(abs_linear16 < table_val)
		index -= 8;
	else
		index += 8;
	table_val = pgm_read_word(&Alaw_expand[index]);

	if(abs_linear16 < table_val)
		index -= 4;
	else
		index += 4;
	table_val = pgm_read_word(&Alaw_expand[index]);
	
	if(abs_linear16 < table_val)
		index -= 2;
	else
		index += 2;
	table_val = pgm_read_word(&Alaw_expand[index]);
	
	if(abs_linear16 < table_val)
		index -= 1;
	else
		index += 1;
	table_val = pgm_read_word(&Alaw_expand[index]);

	if(abs_linear16 < table_val)
		index -= 1;

	// Set correct sign
	if(audio_linear16 < 0)
		index |= 0x80;
	
	return(index);
}

/***************************************************************************
Declaration : int expand_audio(char audio_compressed8)

Description : Expands 8-bit A-law to 16-bit linear using a 128 word
			  A-law compression table.
***************************************************************************/
int expand_audio(char audio_compressed8)
{
int linear16;
	
	// Read linear value in 128 word compression table
	linear16 = pgm_read_word(&Alaw_expand[(audio_compressed8 & 0x7F)]);
	// Check and correct if negative
	if(audio_compressed8 & 0x80)
		return(-linear16);
	else
		return(linear16);
}

/***************************************************************************
Declaration : void test_rf_transmitter(char rf_channel)

Description : Test RF transmitter using CW signal
***************************************************************************/
#ifdef TEST_TX_CW
void test_rf_transmitter(char rf_channel)
{
int i;
char payload[32];

	// Initialize nRF24L01 for CW Test
	init_rf_test();
	for(i=0; i< 32; i++)
		payload[i] = 0;
	write_rf_data(W_TX_PAYLOAD,&payload[0],32);
	reuse_rf_payload();
	
	// Transmit continuously at given frequency
	set_rf_tx(rf_channel);
	RF_ENABLE;
	while(1)
		;
}
#endif

/***************************************************************************
Declaration : void test_rf_modulator(char rf_channel)

Description : Test RF transmitter using modulated signal (1010)
***************************************************************************/
#ifdef TEST_TX_MOD
void test_rf_modulator(char rf_channel)
{
int i;
char payload[32];

	// Initialize nRF24L01 for CW Test
	init_rf_test();
	for(i=0; i< 32; i++)
		payload[i] = 0xAA;
	write_rf_data(W_TX_PAYLOAD,&payload[0],32);
	reuse_rf_payload();
	
	// Transmit continuously at given frequency
	set_rf_tx(rf_channel);
	while(1)
	{
		clear_rf_interrupt();
		rf_enable_pulse();
		wait_rf_irq(TIMEOUT_AUDIO_PACKET,DIV8);
	}
}
#endif

/***************************************************************************
Declaration : void test_rf_receiver(void)

Description : Test RF receiver
***************************************************************************/
#ifdef TEST_RX
void test_rf_receiver(char rf_channel)
{
	// Initialize nRF24L01 for Rx Test
	init_rf_test();
		
	// Receive continuously at given frequency
	set_rf_rx(rf_channel);
	RF_ENABLE;
	while(1)
		;
}
#endif

/***************************************************************************
Declaration : void init_protocol(void)

Description : Initializes Protocol Parameters
***************************************************************************/
void init_protocol(void)
{
	#ifdef DONGLE
		#ifdef USB
			event_times[0] = MASTER_START_TX0/32;
			event_times[1] = MASTER_END_RX0/32;
			event_times[2] = MASTER_START_TX1/32;
			event_times[3] = MASTER_END_RX1/32;
		#else
			event_times[0] = MASTER_START_TX0;
			event_times[1] = MASTER_END_RX0;
			event_times[2] = MASTER_START_TX1;
			event_times[3] = MASTER_END_RX1;
		#endif
		
		protocol_flags = 0;
		
		call_status = 0;
		call_timer = 0;
		call_activity_timer = 0;
		
		signal_in[0] = 0;
		signal_in[1] = 0;
		signal_out[0] = 0;
		signal_out[1] = 0;
		
		packet_loss[0] = 0;
		packet_loss[1] = 0;
		frame_loss = 0;
		
	#endif
	
	#ifdef HEADSET
		event_times[0] = SLAVE_START_RX0;
		event_times[1] = SLAVE_END_RX0;
		event_times[2] = SLAVE_START_TX0;
		event_times[3] = SLAVE_START_RX1;
		event_times[4] = SLAVE_END_RX1;
		event_times[5] = SLAVE_START_TX1;
		
		master_sync_times[0] = SLAVE_RX0_IRQ + TX_RX_DELAY;
		master_sync_times[1] = SLAVE_RX1_IRQ + TX_RX_DELAY;
		
		protocol_flags = 0;
		
		call_status = 0;
		call_timer = 0;
		call_activity_timer = 0;
		
		signal_in[0] = 0;
		signal_in[1] = 0;
		signal_out[0] = 0;
		signal_out[1] = 0;
		
		packet_loss[0] = 0;
		packet_loss[1] = 0;
		frame_loss = 0;
		
	#endif
}

/***************************************************************************
Declaration : void init_freq(void)

Description : Set frequencies to be used in Call setup and Call connection
***************************************************************************/
void init_freq(void)
{
	setup_freq[0] = FREQ_SETUP0;
	setup_freq[1] = FREQ_SETUP1;
	setup_freq[2] = FREQ_SETUP2;
	setup_freq[3] = FREQ_SETUP3;
	
	#ifdef DONGLE
		freq[0] = eeprom_read(EEPROM_ADR_FREQ0);
		freq[1] = eeprom_read(EEPROM_ADR_FREQ1);
		if((freq[0] <= FREQ0_MIN) || (freq[0] > FREQ0_MAX))
			freq[0] = FREQ0_MIN;
		if((freq[1] <= FREQ1_MIN) || (freq[1] > FREQ1_MAX))
			freq[1] = FREQ1_MIN;
	#else
		freq[0] = FREQ0_MIN;
		freq[1] = FREQ1_MIN;
	#endif
}
