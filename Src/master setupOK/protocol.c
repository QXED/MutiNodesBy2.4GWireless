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
#include "stm32f10x_lib.h"

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

char Alaw_compress[2048] =
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
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127
};

/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/
char protocol_flags = 0;
char temp=0;
static unsigned short int event_times[6];
char ctrl_packet_loss[CLIENT_NUM];

char rf_address[TOTAL_ADDR_NUM][ADDRESS_LENGTH]=
{
	{0x11,0xa3,0x94},
	{0x22,0xa3,0x94},
	{0x33,0xa3,0x94},
	{0x44,0xa3,0x94},
	{0x55,0xa3,0x94},
	{0x66,0xa3,0x94}
};

char ctrl_addr_count=0;
int period_count=PERIOD_CTRL;
//static int packet_loss[2];
char  audio_addr_count=0;
int	  rx_addr_num=0;

//  char array_status[4];
char client_id[CLIENT_NUM+1];
char connectstatus;	
char server_id=0x00;
/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/


/***************************************************************************
Declaration : BOOL rf_transceiver(BOOL need_rx)

Description :
***************************************************************************/
bool rf_transceiver(bool need_rx)
{
	int i;
	char status[2];
	unsigned short int *event_time;

	// Exchange Audio Packets at each frequency
	event_time = &event_times[0];
	for(i=0; i<N_FREQ_CONNECTED; i++)
	{
		// Send TX packet synchronized to frame
	 	set_rf_tx(freq[i]);
		temp=read_rf_reg(RF_CH);
	 	clear_rf_interrupt();

		SET_TIMER3_EVENT(*event_time++);
		while(!TIMER3_EVENT) 
			;
	 	LED_TOGGLE;
	 	rf_enable_pulse();
	 	wait_rf_irq(TIMEOUT_AUDIO_PACKET,DIV8);
		
		// Receive Audio RX packet
		set_rf_rx(freq[i]);
		clear_rf_interrupt();
		RF_ENABLE;
		SET_TIMER3_EVENT(*event_time++);
		while(!TIMER3_EVENT && nRF_IRQ)
			;
	 	LED_TOGGLE;
		if(!nRF_IRQ)
			status[i] = STATUS_RX_OK;
		else
			status[i] = STATUS_RX_FAILED;
		RF_DISABLE;
	}
	if((status[0] == STATUS_RX_OK) || (status[1] == STATUS_RX_OK))
		return TRUE;
	else
		return FALSE;
}

/***************************************************************************
Declaration : void setup_transfer(void)

Description : 
***************************************************************************/
void setup_transfer(char freq,char addr_count)
{
	unsigned short int *event_time;
	
	put_setup_packet(addr_count);
	write_rf_data(W_REG|TX_ADDR,&(rf_address[addr_count][0]),ADDRESS_LENGTH);

	event_time = &event_times[0];
	// Send TX packet synchronized to frame
 	set_rf_tx(freq);
	temp=read_rf_reg(RF_CH);
 	clear_rf_interrupt();

	SET_TIMER3_EVENT(*event_time++);
	while(!TIMER3_EVENT) 
		;
 	LED_TOGGLE;
 	rf_enable_pulse();
 	wait_rf_irq(TIMEOUT_AUDIO_PACKET,DIV8);

	  // Receive Audio RX packet
	clear_rf_interrupt();
	set_rf_rx(freq);
	RF_ENABLE;
	SET_TIMER3_EVENT(*event_time++);
	while(!TIMER3_EVENT && nRF_IRQ)
		;
 	LED_TOGGLE;	
	RF_DISABLE;
	if(!nRF_IRQ)
	{	
		if(get_setup_packet(addr_count))	
		{
			connectstatus=CALL_SETUP_OK;
			flush_rf_tx();
			put_setup_packet(addr_count);
			clear_rf_interrupt();
			set_rf_tx(freq);
			rf_enable_pulse();
	 		wait_rf_irq(TIMEOUT_AUDIO_PACKET,DIV8);
		}
	}
	// Flush RX FIFO
	flush_rf_rx();
	flush_rf_tx();
	clear_rf_interrupt();
}

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
//char status[2];
char *freq_ptr;
char setup_addr_count=0;
int i;	
	// Exchange Audio Packets at each frequency
	for(i=0;i<SETUP_TX_ADDR_NUM;i++)
		if(IS_CLIENTID_NULL(client_id[i]))  break;
		
	if(SETUP_TX_ADDR_NUM==i)
		return CALL_SETUP_OK;
	LED_OFF;

	flush_rf_rx();
	flush_rf_tx();
	clear_rf_interrupt();

	freq_ptr=	freq_list;
	for(i=0;i<n_freqs;i++,freq_ptr++)		
//	for(i=0;i<1;i++,freq_ptr++)		
	{
		for(setup_addr_count=0;setup_addr_count<SETUP_TX_ADDR_NUM;)
		{
		  	if(IS_CLIENTID_NULL(client_id[setup_addr_count]))
			{

				if(IS_PERIOD_CTRL(period_count))
				{
					ctrl_transfer();
				}
				else
				{
					setup_transfer(*freq_ptr,setup_addr_count);
					setup_addr_count++;
				}

				period_count++;
				if(period_count>=PERIOD_NUM) period_count=0;
			}
		  	else
		  	{
		  		setup_addr_count++;
			}
		}
	}
	return connectstatus;	
}

/***************************************************************************
Declaration : BOOL audio_transfer(void)

Description : Writes an audio payload to the radio and exchanges audio
			  payloads at the given frequencies. Reads the audio payload
			  from the radio if any is received.
***************************************************************************/
bool audio_transfer(void)
{
	bool rx_success;

	if(PERIOD_1ST_AUDIO==period_count)
		write_rf_data(W_REG|TX_ADDR,&rf_address[audio_addr_count][0],ADDRESS_LENGTH);
	put_audio_packet();
	reuse_rf_payload();

	if(IS_NOT_BC_ADDR_ID(audio_addr_count))
		rx_success=rf_transceiver(TRUE);
	else
		rx_success=rf_transceiver(FALSE);

	// Read RX Audio Packet if any, else stuff a packet
	if(rx_success)
		get_ack_packet(audio_addr_count);

	// Flush RX FIFO
	flush_rf_rx();
	flush_rf_tx();

	rx_addr_num++;
	if(2000==rx_addr_num)
	{
		rx_addr_num=0;
		audio_addr_count++;
		if(audio_addr_count>=TX_ADDR_NUM) // audio_addr_count=0;
		{
			audio_addr_count=0;
		  	return TRUE;
		}
		while(IS_CLIENTID_NULL(client_id[audio_addr_count])) 
			audio_addr_count++;
		if(audio_addr_count>=TX_ADDR_NUM)  audio_addr_count=0;
	}	//Write TX Audio Packet and enable payload re-use
	return FALSE;
}

/***************************************************************************
Declaration : void ctrl_transfer(void)

Description : 
***************************************************************************/
void ctrl_transfer(void)
{
	bool rx_success;

	put_contral_packet();

	reuse_rf_payload();
	if(IS_NOT_BC_ADDR_ID(ctrl_addr_count))
		rx_success=rf_transceiver(TRUE);
	else
		rx_success=rf_transceiver(FALSE);

	if(rx_success)
		get_ack_packet(ctrl_addr_count);
	
	// Flush RX FIFO
	flush_rf_rx();
	flush_rf_tx();
	clear_rf_interrupt();
	
	if(IS_NOT_BC_ADDR_ID(ctrl_addr_count))
	{
		if(rx_success)
		{
			ctrl_packet_loss[ctrl_addr_count]=0;
		}
		else
		{
			ctrl_packet_loss[ctrl_addr_count]++;
			if(ctrl_packet_loss[ctrl_addr_count]>=CTRL_PACKET_LOSS_LIMIT)
			{	
				client_id[ctrl_addr_count]=CLIENTID_NULL;
				ctrl_packet_loss[ctrl_addr_count]=0;
			}
		}
	}

	ctrl_addr_count++;	
	if(ctrl_addr_count>=TX_ADDR_NUM)  ctrl_addr_count=0;
 	while(IS_CLIENTID_NULL(client_id[ctrl_addr_count])) 
		ctrl_addr_count++;	
	if(ctrl_addr_count>=TX_ADDR_NUM)  ctrl_addr_count=0;

}

/***************************************************************************
Declaration : void data_transfer(void)

Description : 
***************************************************************************/
void data_transfer(void)
{
	bool is_break;
	while(1)
	{
		if(IS_PERIOD_CTRL(period_count))
			ctrl_transfer();
		else
			is_break=audio_transfer();
		period_count++;
		if(period_count>=PERIOD_NUM)  period_count=0;
		if(is_break) break;
	}

}

void put_setup_packet(char addr_byte)
{char i;

	ENABLE_RF_SPI;	
	write_rf_byte(W_TX_PAYLOAD);
	write_rf_byte(GET_CLIENT_ID(client_id[addr_byte]));
	write_rf_byte(server_id);
	write_rf_byte(freq[0]);
	write_rf_byte(freq[1]);
	for(i=0; i<28; i++)
	{
		write_rf_byte(0x55);
	}
	DISABLE_RF_SPI;
}

void put_contral_packet(void)
{char i;
//char id[3]={0x6b,0xa3,0x92};
//	write_rf_data(W_REG|RX_ADDR_P0,&(rf_address[ctrl_addr_count][0]),ADDRESS_LENGTH);		// Set Rx and Tx addresses equal to ID
	write_rf_data(W_REG|TX_ADDR,&(rf_address[ctrl_addr_count][0]),ADDRESS_LENGTH);
	
	ENABLE_RF_SPI;	
	write_rf_byte(W_TX_PAYLOAD);
	if(IS_NOT_BC_ADDR_ID(ctrl_addr_count))
		write_rf_byte(client_id[ctrl_addr_count]  | BIT7);
	else
		write_rf_byte(BC_CTRL_CLIENT_ID);
	write_rf_byte(server_id);
	for(i=0; i< AUDIO_SAMPLES; i++)
	{
		write_rf_byte(0x55);
	}

	DISABLE_RF_SPI;
}

/***************************************************************************
Declaration : void put_audio_packet(void)

Description : 
***************************************************************************/
void put_audio_packet(void)
{
short int audio_sample,abs_audio;
//volatile char status;
char audio_byte;
int i;
//char buffer[32];	
	// Put RF Payload
	ENABLE_RF_SPI;
	write_rf_byte(W_TX_PAYLOAD);
	
	// Put Signalling bytes
	if(IS_NOT_BC_ADDR_ID(audio_addr_count)) 
		write_rf_byte(GET_CLIENT_ID(client_id[audio_addr_count]));
	else
		write_rf_byte(BC_AUDIO_CLIENT_ID);
	write_rf_byte(server_id);
	
	// Put audio bytes
	for(i=0; i< AUDIO_SAMPLES; i++)
	{

		// Read 16bit audio sample from input buffer
		audio_sample = ((unsigned short int)(*input_read_ptr++) )<< 8;
		audio_sample += (unsigned short int)(*input_read_ptr++);
		
		input_read_ptr += 2;
		
		if(input_read_ptr >= &input[AUDIO_BUFFER_LENGTH])
			input_read_ptr = &input[0];
		
		// Compress 16bit --> 12bit --> 8bit
		if(audio_sample < 0)
		{
			abs_audio = -audio_sample;
			if(0x8000==audio_sample) abs_audio=0x7FFF;
		}
		else
			abs_audio = audio_sample;
		audio_byte = Alaw_compress[(abs_audio >> 4) & 0x7FF];
		if(audio_sample < 0)
			audio_byte |= 0x80;
		
		// Put audio byte in nRF24L01 FIFO
		write_rf_byte(audio_byte);
	}
	
	if(!(protocol_flags & FLAG_BUFFER_SYNC1))
	{
		input_read_ptr = (char *)((unsigned int)input_write_ptr & 0xFFFFFFFC);
//		input_read_ptr -= 40;
		input_read_ptr -= 148;

		if(input_read_ptr < &input[0])
			input_read_ptr += AUDIO_BUFFER_LENGTH;
		protocol_flags |= FLAG_BUFFER_SYNC1;
	}
	
	DISABLE_RF_SPI;
}

bool get_setup_packet(char addr_id)
{
char i;
char payload[SIGNALING_LEN];

	ENABLE_RF_SPI;
	read_rf_byte(R_RX_PAYLOAD);
	for(i=0;i<SIGNALING_LEN;i++)
		 payload[i] = read_rf_byte(R_RX_PAYLOAD);

	if(SIGNAL_CALL_SETUP_ACK0==payload[1] && SIGNAL_CALL_SETUP_ACK1==payload[2])
	{
		client_id[addr_id]= payload[0] ;
		DISABLE_RF_SPI;
		return (TRUE);
	}
	DISABLE_RF_SPI;
	return (FALSE);
}
/***************************************************************************
Declaration : void get_ack_packet(void)

Description : 
***************************************************************************/
void get_ack_packet(char addr_byte)
{
char i;
char payload[SIGNALING_LEN];

	ENABLE_RF_SPI;
	read_rf_byte(R_RX_PAYLOAD);
	for(i=0;i<SIGNALING_LEN;i++)
		 payload[i] = read_rf_byte(R_RX_PAYLOAD);
	DISABLE_RF_SPI;
	if(server_id != payload[0] || client_id[addr_byte] !=  payload[1]) 	return;
	for(i=0;i<SIGNALING_LEN;i++)
		signal_in[i] = payload[i];		
}

void init_ctrl(void)
{
		ctrl_addr_count=0;	
		period_count=PERIOD_CTRL;
 		server_id=SERVER_ID;
		

		connectstatus=CALL_SETUP_FAILURE;
		client_id[0]=CLIENTID_NULL;
		client_id[1]=CLIENTID_NULL;
		client_id[2]=CLIENTID_NULL;
		client_id[3]=CLIENTID_NULL;
		client_id[4]=server_id;

		ctrl_packet_loss[0]=0;
		ctrl_packet_loss[1]=0;
		ctrl_packet_loss[2]=0;
		ctrl_packet_loss[3]=0;
}
/***************************************************************************
Declaration : void init_protocol(void)

Description : Initializes Protocol Parameters
***************************************************************************/
void init_protocol(void)
{

	event_times[0] = MASTER_START_TX0;
	event_times[1] = MASTER_END_RX0;
	event_times[2] = MASTER_START_TX1;
	event_times[3] = MASTER_END_RX1;
	
	protocol_flags = 0;
	
	call_status = 0;
	call_timer = 0;

	signal_in[0] = 0;
	signal_in[1] = 0;
	signal_out[0] = 0;
	signal_out[1] = 0;
	
	packet_loss[0] = 0;
	packet_loss[1] = 0;
	frame_loss = 0;	
	
	audio_addr_count=0;	
	while(IS_CLIENTID_NULL(client_id[audio_addr_count])) 
		 audio_addr_count++;
	
	connectstatus=CALL_SETUP_OK;
	if(BC_ADDR_ID == audio_addr_count) 
	{
		connectstatus=CALL_SETUP_FAILURE;
	}

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

	if((freq[0] <= FREQ0_MIN) || (freq[0] > FREQ0_MAX))
		freq[0] = FREQ0_MIN;
	if((freq[1] <= FREQ1_MIN) || (freq[1] > FREQ1_MAX))
		freq[1] = FREQ1_MIN;

	freq[0] = 30;
	freq[1] = 50;
}
