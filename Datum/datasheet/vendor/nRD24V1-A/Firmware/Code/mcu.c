/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: mcu.c
INITIAL AUTHOR........: Nordic Semiconductor
INITIAL DATE..........: 15.04.2005
COMPILER..............: avr-gcc (GCC) 3.4.3

Description : Functions representing the MCU resources and initialization.


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version
06.11.2006		1.1			EEPROM routines with enhanced failsafe.
08.11.2006		1.2			USB Suspend signal at PB0 				  
============================================================================*/


/***************************************************************************
* Library Includes
***************************************************************************/
#include <stdio.h>
#include <avr/interrupt.h> 
#include <avr/signal.h>
#include <avr/sleep.h> 
#include <avr/wdt.h> 
#include <avr/pgmspace.h>
#include <avr/io.h>

/***************************************************************************
* Local Includes
***************************************************************************/
#include "mcu.h"

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
Interrupt: SIGNAL(SIG_WATCHDOG_TIMEOUT)

Function : Interrupt routine that is called at Watchdog interrupt.	
***************************************************************************/
SIGNAL(SIG_WATCHDOG_TIMEOUT) 
{
    ;// Simply return after interrupt. 
}

/***************************************************************************
Declaration : void init_mcu (void)

Function :    Initializes ATmega88 MCU
***************************************************************************/
void init_mcu (void)
{
	// Init PORTB
	// PB5 (OUT): SPI_CLK
	// PB4 (IN ): SPI_MISO
	// PB3 (OUT): SPI_MOSI
	// PB2 (OUT): nSPI_RF_ENB / RF_ENB
	// PB1 (OUT): RF_ENB / I2S_SYNC
	// PB0 (OUT): CLKO / USB_SUSPEND
	#ifdef USB
		PORTB = 0b000001;
		DDRB = 0b101110;
	#else
		PORTB = 0b000100;
		DDRB = 0b101111;
	#endif
	// Init PORTC
	// PC6 (IN ): RESET 
	// PC5 (OUT): LED / SCL
	// PC4 (IN ): KBD4 (pull-up enabled) / SDA
	// PC3 (IN ): KBD3 (pull-up enabled) / VOLDN
	// PC2 (IN ): KBD2 (pull-up enabled) / VOLUP
	// PC1 (IN ): KBD1 (pull-up enabled) / MUTEP
	// PC0 (IN ): KBD0 (pull-up enabled) / MUTER
	#ifdef USB
		PORTC = 0b0001111;
		DDRC = 0b0001111;
	#else
		PORTC = 0b0011111;
		DDRC = 0b0100000;
	#endif

	// Init PORTD
	// PD7 (OUT): TEST / nACTIVE
	// PD6 (OUT): nCODEC_RESET / LED
	// PD5 (OUT): nSPI_CODEC_ENB / XMCLK (2048 kHz)
	// PD4 (OUT): I2S_CLK
	// PD3 (OUT): I2S_SYNC / nSPI_RF_ENB
	// PD2 (IN ): nRF_IRQ
	// PD1 (OUT): I2S_OUT
	// PD0 (IN ): I2S_IN
	#ifdef USB
		PORTD = 0b00000000;
		DDRD = 0b01011010;
	#else
		PORTD = 0b01100000;
		DDRD = 0b11111010;
	#endif

}

/***************************************************************************
Declaration : void start_timer0(char timeout, char prescaler)

Description : Set Timer0 to set interrupt flag after timeout clock cycles
***************************************************************************/
void start_timer0(char timeout, char prescaler)
{
	TCCR0A = 0b00000010;		// Mode = CTC
	TCCR0B = 0b00000000;		// Timer0 stopped
	TCNT0 = 0;					// Reset Timer0
	OCR0A = timeout;			// Set Compare Value = Timeout Value
	TIFR0 |= (1<<OCF0A);		// Clear Interrupt Flag (OCF0A)
	TCCR0B = prescaler;			// Start Timer0 with given prescaler
}

/***************************************************************************
Declaration : void start_timer1(unsigned int time, unsigned int period, char prescaler)

Description : Set Timer1 to generate interrupt flag at the given period.
***************************************************************************/
void start_timer1(unsigned int time, unsigned int period, char prescaler)
{
	TCCR1B = 0b00001000;			// Mode = CTC, Timer1 stopped
	TCNT1 = time;					// Set Timer1 to given start value
	OCR1A = period;					// Set Compare Value = Timeout Value
	TIFR1 |= (1<<OCF1A);			// Clear Interrupt Flag (OCF1A)
	TCCR1B = 0x08 | prescaler;		// Start Timer1 with given prescaler
}

/***************************************************************************
Declaration : void init_spi_rf(void)

Description : Enables and initializes the SPI port for RF communication
***************************************************************************/
void init_spi_rf(void)
{
	SPCR = 0b01010000;
	SPSR = 0b00000001;
}

/***************************************************************************
Declaration : void init_spi_codec(void)

Description : Enables and initializes the SPI port for Codec communication
***************************************************************************/
void init_spi_codec(void)
{
	SPCR = 0b01011100;
	SPSR = 0b00000001;
}

/***************************************************************************
Declaration : char spi_byte(char data_byte)

Description : Transmit and receive one byte on the SPI port
***************************************************************************/
char spi_byte(char data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF)))
		;
	return(SPDR);
}

/***************************************************************************
Declaration : void sleep(char wdt_prescaler)

Description : Sleep until WDT interrupt controlled by wdt_prescaler
***************************************************************************/
void sleep(char wdt_prescaler, char mode)
{
	cli();                            			// Disable interrupts
    wdt_reset();								// Watchdog reset
    wdt_prescaler &= 0x07;						// Allow only WDP0,WDP1 and WDP2 to be used
	wdt_prescaler |= (1<<WDIE);				// Select Interrupt Mode
	WDTCSR |= (1<<WDCE) | (1<<WDE);			// Enable changes in WDTCSR
    WDTCSR = wdt_prescaler;             		// Set prescaler
    sei();                                      // Enable interrutps
    set_sleep_mode(mode);						// Set type of sleep mode
	sleep_mode();								// Enter sleep mode	
}

/***************************************************************************
Declaration : void start_usart(void)

Description : Enables the USART for I2S emulation
***************************************************************************/
void start_usart(void)
{
	#ifdef USB
		// Init TIMER/COMPARE1
		OCR1A = 2048/16-1;		// FSYNC = 2*8kHz
		OCR1B = OCR1A;
		
		TCCR1A = 0x80;			// Clear output by forced compare match
		TCCR1C = 0x80;
		
		TCCR1A = 0x40;			// Toggle output on compare match
		TCNT1 = 0;
		
		//Init USART as Master SPI
		UBRR0 = 0;
		UCSR0C = 0xC2;			// Set Master SPI Mode
		UCSR0B = 0x18;			// Enable Transmitter and Receiver
		UBRR0 = 3;				// BCLK = 512kHz
	
		TCCR1B = 0x0F;			// Start Timer1 in CTC Mode using ext. clock
		TIMSK1 |= (1<<OCIE1A);	// Enable Timer1 Compare Interrupt
		sei();
	#else
		//Init TIMER/COMPARE2
		OCR2A = F_CPU/16-1;		// FSYNC = 2*8kHz
		OCR2B = OCR2A;
		
		TCCR2A = 0x22;			// Clear output by forced compare match
		TCCR2B = 0x40;
		
		TCCR2A = 0x12;			// Toggle output on compare match
		TCNT2 = 0;
		
		//Init USART as Master SPI
		UBRR0 = 0;
		UCSR0C = 0xC2;			// Set Master SPI Mode
		UCSR0B = 0x18;			// Enable Transmitter and Receiver
		UBRR0 = 3;				// BCLK = 512kHz
	
		TCCR2B = 0x01;			// Start Timer2 in CTC Mode
		TIMSK2 |= (1<<OCIE2B);	// Enable Timer2 Compare Interrupt
		sei();
	#endif
}

/***************************************************************************
Declaration : void stop_usart(void)
zzzzzzzzzzzzz
Description : Disables the USART for I2S emulation
***************************************************************************/
void stop_usart(void)
{
	#ifdef USB
		TCCR1B = 0x00;				// Stop Timer1
		TIMSK1 &= ~(1<<OCIE1A);	// Disable interrupt
	#else
		TCCR2B = 0x00;				// Stop Timer2
		TIMSK2 &= ~(1<<OCIE2B);	// Disable interrupt
	#endif
	UCSR0B = 0x00;				// Disable Transmitter and Receiver
	UCSR0B &= ~(1<<UDRIE0);	// Disable USART Data Empty Interrupt

}

/***************************************************************************
Declaration : char eeprom_read(unsigned int address)		

Description : Reads one byte of data in EEPROM at given address   
***************************************************************************/
char eeprom_read(unsigned int address)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE))
		;
	
	/* Set up address register */
	EEAR = address;
	
	/* Read EEPROM data */
	EECR |= (1<<EERE);
	
	/* Set EEPROM Address to unused location */
	EEAR = 0xFF;
	
	return(EEDR);
}

/***************************************************************************
Declaration : void eeprom_write(char data,unsigned int address)		

Description : Writes one byte of data in EEPROM at given address   
***************************************************************************/
void eeprom_write(char data,unsigned int address)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE))
		;

	/* Set up address and data register */
	EEAR = address;
	EEDR = data;
	
	/* Enable EEPROM write */
	EECR |= (1<<EEMWE);
	
	/* Write EEPROM */
	EECR |= (1<<EEWE);
	
	/* Set EEPROM Address to unused location */
	EEAR = 0xFF;
}

