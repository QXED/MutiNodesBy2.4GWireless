/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: mcu.h
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : 


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
07.11.2006		1.1			Updated Sleep Mode Constant Definitions
08.11.2006		1.2			USB Suspend macro				  
============================================================================*/
#ifndef MCU_H
#define MCU_H

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
// General
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

// Oscillator
#ifdef USB
#define F_CPU				6000		// Frequency in kHz
#else
#define F_CPU				4096		// Frequency in kHz
#endif

// Sleep Modes
#define IDLE_MODE			2
#define POWER_DOWN_MODE		4
#define STANDBY_MODE		12

// Watchdog Timeout
#define  WDT_TIMEOUT_15MS   0 
#define  WDT_TIMEOUT_30MS   1 
#define  WDT_TIMEOUT_60MS   2 
#define  WDT_TIMEOUT_120MS  3 
#define  WDT_TIMEOUT_250MS  4 
#define  WDT_TIMEOUT_500MS  5 
#define  WDT_TIMEOUT_1S   	6 
#define  WDT_TIMEOUT_2S   	7 

// Timer prescaler
#define DIV1				1
#define DIV8				2
#define	DIV64				3
#define	DIV256				4
#define	DIV1024				5

/***************************************************************************
* Types 
***************************************************************************/

/***************************************************************************
* Variables 
***************************************************************************/

/***************************************************************************
* Macros 
***************************************************************************/
// ATmega88 - Timers
#define TIMER0_TIMEOUT				(TIFR0 & (1<<OCF0A))
#define TIMER1_TIMEOUT				(TIFR1 & (1<<OCF1A))
#define TIMER1_CLEAR_FLAG 			(TIFR1 |= (1<<OCF1A))
#define TIMER1_EVENT				(TIFR1 & (1<<OCF1B))
#define SET_TIMER1(data_word)		TCNT1 = data_word
#define SET_TIMER1_EVENT(data_word)	TIFR1 |= (1<<OCF1B); \
									OCR1B = data_word
#define TIMER2_EVENT				(TIFR2 & (1<<OCF2A))
#define SET_TIMER2_EVENT(data_byte)	TIFR2 |= (1<<OCF2A); \
									OCR2A = data_byte;
#define START_TIMER2				TCCR2B = 0x03
#define STOP_TIMER2					TCCR2B = 0x00; \
									GTCCR |= (1<<PSRASY); \
									TCNT2 = 0x00; \
									TIFR2 |= (1<<OCF2A)
									
// ATmega88 - SPI
#define WRITE_SPI(data_byte)		SPDR = data_byte;
#define READ_SPI(data_byte)			data_byte = SPDR;
#define READ_SPI_STATUS(data_byte)	data_byte = SPSR;
#define WAIT_SPI_READY				while(!(SPSR & (1<<SPIF))) \
										;

// nRF24L01
#ifdef USB
	#define ENABLE_RF_SPI	(PORTD &= ~BIT3)
	#define DISABLE_RF_SPI	(PORTD |= BIT3)
	#define RF_ENABLE		(PORTB |= BIT2)
	#define RF_DISABLE		(PORTB &= ~BIT2)
#else
	#define ENABLE_RF_SPI	(PORTB &= ~BIT2)
	#define DISABLE_RF_SPI	(PORTB |= BIT2)
	#define RF_ENABLE		(PORTB |= BIT1)
	#define RF_DISABLE		(PORTB &= ~BIT1)
#endif
#define nRF_IRQ			(PIND & BIT2)

// Codec XE3005
#define ENABLE_CODEC_RESET	(PORTD &= ~BIT6)
#define DISABLE_CODEC_RESET	(PORTD |= BIT6)
#define ENABLE_CODEC_SPI	(PORTD &= ~BIT5)
#define DISABLE_CODEC_SPI	(PORTD |= BIT5)
#define FSYNC				(PIND & BIT3)
#define SET_FSYNC_NEXT		TCCR2A = 0x32
#define CLEAR_FSYNC_NEXT	TCCR2A = 0x22

// USB Controller
#define CALL_ACTIVITY_PIN	(PIND & BIT7)
#define USB_SUSPEND			(PINB & BIT0)
#define SET_VOLUME_DOWN		(PORTC |= BIT3)		
#define CLEAR_VOLUME_DOWN	(PORTC &= ~BIT3)
#define SET_VOLUME_UP		(PORTC |= BIT2)
#define CLEAR_VOLUME_UP		(PORTC &= ~BIT2)
#define SET_MUTE_PLAY		(PORTC |= BIT1)
#define CLEAR_MUTE_PLAY		(PORTC &= ~BIT1)
#define SET_MUTE_REC		(PORTC |= BIT0)
#define CLEAR_MUTE_REC		(PORTC &= ~BIT0)

// Keys
#define KBD0			(PINC & BIT0)
#define KBD1			(PINC & BIT1)
#define KBD2			(PINC & BIT2)
#define KBD3			(PINC & BIT3)
#define KBD4			(PINC & BIT4)

// LED
#ifdef USB
	#define LED_ON			(PORTD |= BIT6)
	#define LED_OFF			(PORTD &= ~BIT6)
	#define LED_TOGGLE		(PORTD ^= BIT6)
#else
	#define LED_ON			(PORTC |= BIT5)
	#define LED_OFF			(PORTC &= ~BIT5)
	#define LED_TOGGLE		(PORTC ^= BIT5)
#endif

/***************************************************************************
* Prototypes
***************************************************************************/
void init_mcu(void);
void init_spi_rf(void);
void init_spi_codec(void);
char spi_byte(char data_byte);
void start_timer1(unsigned int time, unsigned int period, char prescaler);
void start_timer0(char timeout, char prescaler);
void sleep(char wdt_prescaler, char mode);
void start_usart(void);
void stop_usart(void);
char eeprom_read(unsigned int address);
void eeprom_write(char data,unsigned int address);
#endif
