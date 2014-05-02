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

// Sleep Modes
#define IDLE_MODE			2
#define POWER_DOWN_MODE		4
#define STANDBY_MODE		12

// Watchdog Timeout
#define  WDT_TIMEOUT_15MS   491 
#define  WDT_TIMEOUT_30MS   983 
#define  WDT_TIMEOUT_60MS   1966 
#define  WDT_TIMEOUT_120MS  3932 
#define  WDT_TIMEOUT_250MS  8192 
#define  WDT_TIMEOUT_500MS  16384 
#define  WDT_TIMEOUT_1S   	32768 

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
#define TIMER4_TIMEOUT				(TIM_GetITStatus(TIM4, TIM_IT_Update)!= RESET)
//#define TIMER1_TIMEOUT				(TIFR1 & (1<<OCF1A))
//#define TIMER1_CLEAR_FLAG 			(TIFR1 |= (1<<OCF1A))
#define TIMER3_EVENT				(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
#define SET_TIMER3(data_word)		(TIM_SetCounter(TIM3, data_word))
#define SET_TIMER3_EVENT(data_word) 	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); \
										TIM_SetCompare1(TIM3,data_word)

//#define TIMER2_EVENT				(TIFR2 & (1<<OCF2A))
//#define SET_TIMER2_EVENT(data_byte)	TIFR2 |= (1<<OCF2A); \
//									OCR2A = data_byte;
//#define START_TIMER2				TCCR2B = 0x03
//#define STOP_TIMER2					TCCR2B = 0x00; \
//									GTCCR |= (1<<PSRASY); \
//									TCNT2 = 0x00; \
//									TIFR2 |= (1<<OCF2A)
									
// ATmega88 - SPI
#define WRITE_SPI(data_byte)		while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET); \
									SPI_SendData(SPI1, data_byte)//SPDR = data_byte;
#define READ_SPI(data_byte)			data_byte = SPI_ReceiveData(SPI1)
#define READ_SPI_STATUS(data_byte)	data_byte = SPSR
#define WAIT_SPI_READY				 while(SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET) 
										

// nRF24L01
#define ENABLE_RF_SPI	GPIO_ResetBits(GPIOA, GPIO_Pin_4)//cs low
#define DISABLE_RF_SPI	GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define RF_ENABLE		GPIO_SetBits(GPIOB, GPIO_Pin_0)//ce high
#define RF_DISABLE		GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define nRF_IRQ			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)


// Codec XE3005
//#define ENABLE_CODEC_RESET	(PORTD &= ~BIT6)
//#define DISABLE_CODEC_RESET	(PORTD |= BIT6)
//#define ENABLE_CODEC_SPI	(PORTD &= ~BIT5)
//#define DISABLE_CODEC_SPI	(PORTD |= BIT5)
//#define FSYNC				(PIND & BIT3)
//#define SET_FSYNC_NEXT		TCCR2A = 0x32
//#define CLEAR_FSYNC_NEXT	TCCR2A = 0x22

// Keys
//#define KBD0			(PINC & BIT0)
//#define KBD1			(PINC & BIT1)
//#define KBD2			(PINC & BIT2)
//#define KBD3			(PINC & BIT3)
//#define KBD4			(PINC & BIT4)

// LED
#define LED_ON			GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define LED_OFF			GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define LED_TOGGLE		GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10)))


void RCC_Configuration(void);
void  USART1_Configuration(void);
void SPI_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void TIM2_Configuration(void);
void TIM3_Configuration(void);
void TIM4_Configuration(void);
void RTC_Configuration(void); 
void NVIC_Configuration(void);
void EXTI_Configuration(void);

void init_mcu(void);
void start_timer4(unsigned short timeout);
void start_timer3(void);
void init_spi_rf(void);
void SYSCLKConfig_STOP(void);
void init_spi_codec(void);

char spi_byte(char data_byte);

void sleep(int data_ms, char mode) ;

void start_usart(void);

void stop_usart(void);

char eeprom_read(unsigned short address);

void eeprom_write(char data,unsigned short address);

#endif
