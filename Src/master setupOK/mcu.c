/*============================================================================
PROJECT...............: Wireless Headset
FILE..................: mcu.c
INITIAL AUTHOR........: Fly.Zeng GoerTek
INITIAL DATE..........: 18.10.2007
COMPILER..............: Keil uVision3

Description : Functions representing the MCU resources and initialization.


Modification history:

DATE      		VERSION		COMMENT
-----------------------------------------------------------------------------
03.11.2006		1.0			Initial version				  
============================================================================*/


/***************************************************************************
* Library Includes
***************************************************************************/
#include "stm32f10x_lib.h"

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
ErrorStatus HSEStartUpStatus;

/***************************************************************************
* Macros 
***************************************************************************/

/***************************************************************************
* Prototypes
***************************************************************************/

/***************************************************************************
Declaration : void init_mcu (void)

Function :    Initializes ATmega88 MCU
***************************************************************************/
void init_mcu (void)
{
#ifdef DEBUG
  debug();
#endif
  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* Configure the GPIO ports */
  GPIO_Configuration();
 
  /* Configure EXTI Line9 to generate an interrupt on falling edge */
  EXTI_Configuration();

  /* SPI the GPIO ports */
  SPI_Configuration(); 
   
  /* USART  Configuration as SPI */
  USART1_Configuration();
  
  /* TIM2 Configuration */
  TIM2_Configuration();
  
  /* TIM3 Configuration */
  TIM3_Configuration();
  
  /* TIM4 Configuration */
  TIM4_Configuration();
  
   /* Configure RTC clock source and prescaler */
  RTC_Configuration(); 
   
  /* NVIC configuration */
  NVIC_Configuration();
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK/1 */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
 
    /* ADCCLK = PCLK2/8 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

 /* Enable peripheral clocks --------------------------------------------------*/
  /* Periph clock enable 72M max*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |  RCC_APB2Periph_USART1 |
                         RCC_APB2Periph_SPI1 | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);

  /*  Periph clock enable 36M max*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |
  			   RCC_APB1Periph_TIM4 | RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}


/*******************************************************************************
* Function Name  : SYSCLKConfig_STOP
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}


/* USART1 configuration*****************************************************
* USART1 configured as follow:
*        - BaudRate = 230400 baud  
*        - Word Length = 8 Bits
*        - One Stop Bit
*        - No parity
*        - Hardware flow control disabled (RTS and CTS signals)
*        - Receive and transmit enabled
*        - USART Clock Enabled
*        - USART CPOL: Clock is active Low
*        - USART CPHA: Data is captured on the first edge 
*        - USART LastBit: The clock pulse of the last data bit is output to 
*                         the SCLK pin
**************************************************************************/
void  USART1_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 230400;//115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Clock = USART_Clock_Enable;
  //USART_InitStructure.USART_CPOL = USART_CPOL_High;
  //USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_CPOL = SPI_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_1Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Enable;
  //USART_Init(USART1, &USART_InitStructure);
  
  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);

}

/*******************************************************************************
* Function Name  : SPI_Configuration
* Description    : Configures the SPI2 to DAC and SPI1 to RF.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Configuration(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

 
  //SPI_CSFLASHALL_HIGH();
  DISABLE_RF_SPI;
  // SPI2 configuration TO DAC ------------------------------------------------------

	 SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	 SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	 SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//fCPU=36Mhz really measuring value & BaudRate=562.5kHz
	 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	 SPI_InitStructure.SPI_CRCPolynomial = 7;
	 SPI_Init(SPI2, &SPI_InitStructure);
	 SPI_Cmd(SPI2, ENABLE);   

	// SPI1 configuration TO RF------------------------------------------------------
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	//SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
       //SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//64
	//SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	//SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
 	SPI_Cmd(SPI1, ENABLE); 
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_4 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  /* Configure SPI2 pins--TO dac: SCK,  MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;// GPIO_Pin_14 | 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_10 | GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	  /* Configure PB1 as input floating (EXTI Line1) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{

 NVIC_InitTypeDef NVIC_InitStructure;

  #ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
  #else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  #endif

  /* 1 bit for pre-emption priority, 3 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the TIM2 global Interrupt */   
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

 // NVIC->Disable[EXTI9_5_IRQChannel >> 0x05] =
 //    (u32)0x01 << (EXTI9_5_IRQChannel & (u8)0x1F);

}


/*******************************************************************************
* Function Name  : EXTI_Configuration
* Description    : Configures EXTI Line9 and Line17(RTC Alarm).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);    
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures RTC clock source and prescaler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
  /* RTC clock source configuration ------------------------------------------*/
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();
  
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* RTC configuration -------------------------------------------------------*/
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Set the RTC time base to 1s */
  RTC_SetPrescaler(0);  
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Alarm interrupt */
//  RTC_ITConfig(RTC_IT_ALR, ENABLE);
  /* Wait until last write operation on RTC registers has finished */
//  RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : TIM2_Configuration
* Description    : Configures TIM2 output  8K as WS of DAC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_Configuration(void)
{
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;
 
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;          
  TIM_TimeBaseStructure.TIM_Prescaler = TIM2_Pre;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;          
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_2;          
  TIM_OCInitStructure.TIM_Pulse = TIM2_CCR2_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInit(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
 // TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);//
}

/*******************************************************************************
* Function Name  : TIM3_Configuration
* Description    : Configures TIM3 as sync_times.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  TIM_TimeBaseStructure.TIM_Period = FRAME_PERIOD;          
  TIM_TimeBaseStructure.TIM_Prescaler = TIM3_Pre;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;          
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_1;          
  TIM_OCInitStructure.TIM_Pulse = FRAME_PERIOD-1;  
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInit(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

 // TIM_ARRPreloadConfig(TIM3,DISABLE);
}

/*******************************************************************************
* Function Name  : TIM4_Configuration
* Description    : Configures TIM4.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  TIM_TimeBaseStructure.TIM_Period = 0xffff;          
  TIM_TimeBaseStructure.TIM_Prescaler = TIM4_Pre;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM4,ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
}


/***************************************************************************
Declaration : void start_timer0(char timeout, char prescaler)

Description : Set Timer0 to set interrupt flag after timeout clock cycles
***************************************************************************/
void start_timer4(unsigned short timeout)
{  
   //TIM_Cmd(TIM4, DISABLE);
   
   //TIM_SetCompare1(TIM4, timeout);
   
   //TIM_Cmd(TIM4, ENABLE);
   TIM_SetAutoreload(TIM4, timeout);
   //TIM_Cmd(TIM4, ENABLE);
   TIM_GenerateEvent(TIM4,TIM_EventSource_Update);
   TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

/***************************************************************************
Declaration :void start_timer3(void)
Description : Set Timer3 to generate interrupt flag at the given period.
***************************************************************************/
void start_timer3(void)
{
   //TIM3->EGR |= TIM_EventSource_Update;
   TIM_Cmd(TIM3, ENABLE);
   //TIM_GenerateEvent(TIM3,TIM_EventSource_Update);
   TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
}

/***************************************************************************
Declaration : void init_spi_rf(void)

Description : Enables and initializes the SPI port for RF communication
***************************************************************************/
void init_spi_rf(void)
{
	DISABLE_RF_SPI;
	RF_DISABLE;
}

/***************************************************************************
Declaration : void init_spi_codec(void)

Description : Enables and initializes the SPI port for Codec communication
***************************************************************************/
void init_spi_codec(void)
{
	
}

/***************************************************************************
Declaration : char spi_byte(char data_byte)

Description : Transmit and receive one byte on the SPI port
***************************************************************************/
char spi_byte(char data_byte)
{
  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET);
  /* Send byte through the SPI1 peripheral */
  SPI_SendData(SPI1, data_byte);

  /* Wait to receive a byte */
  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET);
  data_byte=SPI_ReceiveData(SPI1);
  /* Return the byte read from the SPI bus */
  return data_byte;	
}

/***************************************************************************
Declaration : void sleep(char wdt_prescaler)

Description : Sleep until WDT interrupt controlled by wdt_prescaler
***************************************************************************/
void sleep(int data_ms, char mode)
{
   /* Enable the RTC Alarm interrupt */
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Alarm in data_ms */
 //   RTC_SetAlarm(RTC_GetCounter()+ data_ms<<5);

    RTC_SetAlarm(RTC_GetCounter()+ data_ms);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Request to enter STOP mode with regulator ON */
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
    
    /* At this stage the system has resumed from STOP mode -------------------*/

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
       as system clock source (HSE and PLL are disabled in STOP mode) */
    /* Enable the RTC Alarm interrupt */
    SYSCLKConfig_STOP();
    RTC_ITConfig(RTC_IT_ALR, DISABLE);
   /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
	

	

}

/***************************************************************************
Declaration : void start_usart(void)

Description : Enables the USART for I2S emulation
***************************************************************************/
void start_usart(void)
{
	
}

/***************************************************************************
Declaration : void stop_usart(void)

Description : Disables the USART for I2S emulation
***************************************************************************/
void stop_usart(void)
{


}

/***************************************************************************
Declaration : char eeprom_read(unsigned int address)		

Description : Reads one byte of data in EEPROM at given address   
***************************************************************************/
char eeprom_read(unsigned short address)
{
 return  (address & 0xff);	
}

/***************************************************************************
Declaration : void eeprom_write(char data,unsigned int address)		

Description : Writes one byte of data in EEPROM at given address   
***************************************************************************/
void eeprom_write(char data,unsigned short address)
{
	
}

