PA.1--TIM2 Channel2  as Output IIS-WS
PA.11--FLASH1 CS OUT PUT
PA.12--FLASH2 CS OUT PUT
PB.12--FLASH3 CS OUT PUT
PB.1 --RF IRQ IN PUT
PB.0 --RF CE OUT PUT
PA.4--RF CS OUT PUT

Configure SPI1 pins-TO RF(FLASH): SCK, MISO and MOSI
PA.5--SPI1 SCK
PA.6--SPI1 MISO
PA.7--SPI1 MOSI

Configure SPI1 pins-GPIO TO FLASH  	 GPIO_SPI
PA.8 as Output used as SPIflash-CLK
PA.9 as Output push-pull, used as SPIflash-MOSI
PA.10 as Input used as SPIflash-MISO

Configure SPI1 pins-USART1 TO FLASH  USART_SPI
PA.9--MOSI(Tx)
PA.8--SCK(CK)
PA.10--MISO(Rx) 

Configure SPI2 pins--TO dac: SCK,  MOSI
PB.13--SPI2  SCK
PB.15--SPI2  MOSI

PB.5 OUTPUT LED
PA.2 VBB-AD



看门狗模块（自减计数器，重载计数常量实现）
ADC模块（AD看门狗实现对电压的监测）
定时器模块
射频模块（SPI口对2401编程实现）
flash模块 （USART口仿SPI口读写FLAHS实现）

编解码模块


1、GPIO口配置  GPIO_Configuration
2、IWDG配置 IWDG_Configuration
3、EXTI配置EXTI_Configuration
4、USART口配置USART1_configuration
5、DMA方式配置 DMA_spiConfiguration和DMA_usartConfiguration
6、ADC配置 ADC_AWDconfiguration
7、TIM配置 TIM2_Configuration
8、RCC配置 RCC_Configuration
9、NVIC配置 NVIC_Configuration
10、IRQ函数 stm32f10x_it.c

main.c         主函数              片内外设各个功能模块初始化
spi_flash.c    FLASH读写子函数     DMA、通用方式读写FLASH数据、ID
radio.c        RF模块读写子函数    RF芯片寄存器读写、RF模块初始化等
spi.c          SPI底层函数         SPI口配置、为FLASH、RF提供底层驱动
stm32f10x_it.c 系统中断函数        定时器中断、RF接收中断、DMA(FLASH、RF)完成中断和ADC中断
STM32F10x.s    系统入口 芯片初始化 中断向量表初始化 C语言main函数的跳转   