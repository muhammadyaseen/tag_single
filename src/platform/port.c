/*! ----------------------------------------------------------------------------
 * @file	port.c
 * @brief	HW specific definitions and functions for portability
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include "compiler.h"
#include "port.h"

#define rcc_init(x)					RCC_Configuration(x)
#define timer_init(x)				Timer_Configuration(x)
#define systick_init(x)				SysTick_Configuration(x)
#define rtc_init(x)					RTC_Configuration(x)
#define interrupt_init(x)			NVIC_Configuration(x)
#define usart_init(x)				USART_Configuration(x)
#define spi_init(x)					SPI_Configuration(x)
#define gpio_init(x)				GPIO_Configuration(x)
#define ethernet_init(x)			No_Configuration(x)
#define fs_init(x)					No_Configuration(x)
#define usb_init(x)					No_Configuration(x)
#define lcd_init(x)					No_Configuration(x)
#define touch_screen_init(x)		No_Configuration(x)



/* System tick 32 bit variable defined by the platform */
extern __IO unsigned long time32_incr;

int No_Configuration(void)
{
	return -1;
}

int portGetTickCnt(void)
{
	return time32_incr;
}

int SysTick_Configuration(void)
{
	if (SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC))
	{
		/* Capture error */
		while (1);
	}
	NVIC_SetPriority (SysTick_IRQn, 5);

	return 0;
}

void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}


int NVIC_DisableDECAIRQ(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure EXTI line */
	EXTI_InitStructure.EXTI_Line = DECAIRQ_EXTI;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MPW3 IRQ polarity is high by default
	EXTI_InitStructure.EXTI_LineCmd = DECAIRQ_EXTI_NOIRQ;
	EXTI_Init(&EXTI_InitStructure);

	return 0;
}


int NVIC_Configuration(void)
{


	setup_RoleBtnIRQ();

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable GPIO used as DECA IRQ for interrupt
	GPIO_InitStructure.GPIO_Pin = DECAIRQ;
	GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_IPD;	//IRQ pin should be Pull Down to prevent unnecessary EXT IRQ while DW1000 goes to sleep mode
	GPIO_Init(DECAIRQ_GPIO, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(DECAIRQ_EXTI_PORT, DECAIRQ_EXTI_PIN);

	/* Configure EXTI line */
	EXTI_InitStructure.EXTI_Line = DECAIRQ_EXTI;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MPW3 IRQ polarity is high by default
	EXTI_InitStructure.EXTI_LineCmd = DECAIRQ_EXTI_USEIRQ;
	EXTI_Init(&EXTI_InitStructure);

	/* Set NVIC Grouping to 16 groups of interrupt without sub-grouping */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Enable and set EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = DECAIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DECAIRQ_EXTI_USEIRQ;

	NVIC_Init(&NVIC_InitStructure);

	/* Enable the RTC Interrupt */
	//NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	//NVIC_Init(&NVIC_InitStructure);

	return 0;
}

void setup_RoleBtnIRQ()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable GPIO used as ROLE select for interrupt  - PC1
	GPIO_InitStructure.GPIO_Pin = ROLEBTN;
	GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_IPD;	//IRQ pin should be Pull Down to prevent unnecessary EXT IRQ while DW1000 goes to sleep mode
	GPIO_Init(ROLEBTN_GPIO, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);

	/* Configure EXTI line */
	EXTI_InitStructure.EXTI_Line = ROLEBTN_EXTI;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//MP IRQ polarity is high by default
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Set NVIC Grouping to 16 groups of interrupt without sub-grouping */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Enable and set EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

}


void role_btn_set(FunctionalState command)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	/* Configure EXTI line */
	EXTI_InitStructure.EXTI_Line = ROLEBTN_EXTI;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//MP IRQ polarity is high by default
	EXTI_InitStructure.EXTI_LineCmd = command;
	EXTI_Init(&EXTI_InitStructure);
}

/**
  * @brief  Checks whether the specified EXTI line is enabled or not.
  * @param  EXTI_Line: specifies the EXTI line to check.
  *   This parameter can be:
  *     @arg EXTI_Linex: External interrupt line x where x(0..19)
  * @retval The "enable" state of EXTI_Line (SET or RESET).
  */
ITStatus EXTI_GetITEnStatus(uint32_t EXTI_Line)
{
  ITStatus bitstatus = RESET;
  uint32_t enablestatus = 0;
  /* Check the parameters */
  assert_param(IS_GET_EXTI_LINE(EXTI_Line));

  enablestatus =  EXTI->IMR & EXTI_Line;
  if (enablestatus != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

int RCC_Configuration(void)
{
	//SetSysClockToHSE();
		ErrorStatus HSEStartUpStatus;
		RCC_ClocksTypeDef RCC_ClockFreq;

		/* RCC system reset(for debug purpose) */
		RCC_DeInit();

		/* Enable HSE */
		RCC_HSEConfig(RCC_HSE_ON);

		/* Wait till HSE is ready */
		HSEStartUpStatus = RCC_WaitForHSEStartUp();

		if(HSEStartUpStatus != ERROR)
		{
			/* Enable Prefetch Buffer */
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

			/****************************************************************/
			/*      HSE = 16 MHz, HCLK = 72 MHz, PCLK2 = 72 MHz, PCLK1 = 36 MHz         */
			/****************************************************************/
			/* Flash 2 wait state */
			//FLASH_SetLatency(FLASH_Latency_2);
			FLASH_SetLatency(FLASH_Latency_0);
			/* HCLK = SYSCLK */
			RCC_HCLKConfig(RCC_SYSCLK_Div1);
			/* PCLK2 = HCLK */
			RCC_PCLK2Config(RCC_HCLK_Div1);
			/* PCLK1 = HCLK/2 */
			RCC_PCLK1Config(RCC_HCLK_Div2);
			/*  ADCCLK = PCLK2/4 */
			RCC_ADCCLKConfig(RCC_PCLK2_Div4);

			/* Configure PLLs *********************************************************/
			/* PLL2 configuration: PLL2CLK = (HSE / 4) * 8 = 24 MHz 			(16/8)*12 = 24 MHz*/
			//RCC_PREDIV2Config(RCC_PREDIV2_Div4);
			//RCC_PLL2Config(RCC_PLL2Mul_8);
			RCC_PREDIV2Config(RCC_PREDIV2_Div8);
			RCC_PLL2Config(RCC_PLL2Mul_12);


			/* Enable PLL2 */
			RCC_PLL2Cmd(ENABLE);

			/* Wait till PLL2 is ready */
			while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET){}

			/* PLL1 configuration: PLLCLK = (PLL2 / 3) * 9 = 72 MHz
			RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div3);

			RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);

			/* Enable PLL */
			RCC_PLLCmd(ENABLE);

			/* Wait till PLL is ready */
			while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

			/* Select PLL as system clock source */
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

			/* Wait till PLL is used as system clock source */
			while (RCC_GetSYSCLKSource() != 0x08){}
		}

		RCC_GetClocksFreq(&RCC_ClockFreq);

		/* Enable SPI1 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

		/* Enable SPI2 clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

		/* Enable GPIOs clocks */
		RCC_APB2PeriphClockCmd(
							RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
							RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO  |
							RCC_APB2Periph_USART1 ,
							ENABLE);
	//MCO Pin Setup

//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	RCC_MCOConfig(RCC_MCO_HSE);

	return 0;
}

int USART_Configuration(void)
{

	//USART_DeInit(USART1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	USART_Cmd(USART1, ENABLE);

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	// USARTx setup
	USART_InitStructure.USART_BaudRate = 300;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	//USART_Init(USARTx, &USART_InitStructure);
	USART_Init(USART1, &USART_InitStructure);

	// USARTx TX pin setup
	//GPIO_InitStructure.GPIO_Pin = USARTx_TX;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	//GPIO_Init(USARTx_GPIO, &GPIO_InitStructure);
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USARTx RX pin setup
//	GPIO_InitStructure.GPIO_Pin = USARTx_RX;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	//GPIO_Init(USARTx_GPIO, &GPIO_InitStructure);
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Enable USARTx
	//USART_Cmd(USARTx, ENABLE);
	USART_Cmd(USART1, ENABLE);

    return 0;
}

void SPI_ChangeRate(uint16_t scalingfactor)
{
	uint16_t tmpreg = 0;

	/* Get the SPIx CR1 value */
	tmpreg = SPIx->CR1;

	/*clear the scaling bits*/
	tmpreg &= 0xFFC7;

	/*set the scaling bits*/
	tmpreg |= scalingfactor;

	/* Write to SPIx CR1 */
	SPIx->CR1 = tmpreg;
}

void SPI_ConfigFastRate(uint16_t scalingfactor)
{
	SPI_InitTypeDef SPI_InitStructure;

	SPI_I2S_DeInit(SPIx);

	// SPIx Mode setup
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	 //
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = scalingfactor; //sets BR[2:0] bits - baudrate in SPI_CR1 reg bits 4-6
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPIx, &SPI_InitStructure);

	// Enable SPIx
	SPI_Cmd(SPIx, ENABLE);
}

int SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	SPI_I2S_DeInit(SPIx);

	// SPIx Mode setup
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	 //
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //sets BR[2:0] bits - baudrate in SPI_CR1 reg bits 4-6
	SPI_InitStructure.SPI_BaudRatePrescaler = SPIx_PRESCALER;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPIx, &SPI_InitStructure);

	// SPIx SCK and MOSI pin setup
	GPIO_InitStructure.GPIO_Pin = SPIx_SCK | SPIx_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);

	// SPIx MISO pin setup
	GPIO_InitStructure.GPIO_Pin = SPIx_MISO;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

	GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);

	// SPIx CS pin setup
	GPIO_InitStructure.GPIO_Pin = SPIx_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(SPIx_CS_GPIO, &GPIO_InitStructure);

	// Disable SPIx SS Output
	SPI_SSOutputCmd(SPIx, DISABLE);

	// Enable SPIx
	SPI_Cmd(SPIx, ENABLE);

	// Set CS high
	GPIO_SetBits(SPIx_CS_GPIO, SPIx_CS);

    return 0;
}


int SPI2_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//SPI_I2S_DeInit(SPIy);
	SPI_I2S_DeInit(SI_SPI);

	// SPIy Mode setup
	//SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	 //
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	//SPI_InitStructure.SPI_BaudRatePrescaler = SPIy_PRESCALER;
	SPI_InitStructure.SPI_BaudRatePrescaler = SI_BautRatePrescaler;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SI_SPI, &SPI_InitStructure);

	// SPIy SCK and MOSI pin setup
	//GPIO_InitStructure.GPIO_Pin = SPIy_SCK | SPIy_MOSI;
	GPIO_InitStructure.GPIO_Pin = SI_SCK | SI_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(SI_GPIO, &GPIO_InitStructure);

	// SPIy MISO pin setup
	//GPIO_InitStructure.GPIO_Pin = SPIy_MISO;
	GPIO_InitStructure.GPIO_Pin = SI_MISO;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

	//GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
	GPIO_Init(SI_GPIO, &GPIO_InitStructure);

	// SPIy CS pin setup
	//GPIO_InitStructure.GPIO_Pin = SPIy_CS;
	GPIO_InitStructure.GPIO_Pin = SI_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	//GPIO_Init(SPIy_CS_GPIO, &GPIO_InitStructure);
	GPIO_Init(SI_GPIO, &GPIO_InitStructure);

	// Disable SPIy SS Output
	//SPI_SSOutputCmd(SPIy, DISABLE);
	//SPI_SSOutputCmd(SI_SPI, DISABLE);

	// Enable SPIy
	//SPI_Cmd(SPIy, ENABLE);
	SPI_Cmd(SI_SPI, ENABLE);

	// Set CS high
	//GPIO_SetBits(SPIy_CS_GPIO, SPIy_CS);
	GPIO_SetBits(SI_GPIO, SI_CS);

	// LCD_RS pin setup
//	GPIO_InitStructure.GPIO_Pin = LCD_RS;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//	GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
//
//	// LCD_RW pin setup
//	GPIO_InitStructure.GPIO_Pin = LCD_RW;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//	GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);

    return 0;
}


int GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure all unused GPIO port pins in Analog Input mode (floating input
	* trigger OFF), this will reduce the power consumption and increase the device
	* immunity against EMI/EMC */

	// Enable GPIOs clocks
	RCC_APB2PeriphClockCmd(
						RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
						RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,
						ENABLE);

	// Set all GPIO pins as analog inputs
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);

#ifdef SAFETY_VEST

	// battery to ADC connector switch
//	GPIO_InitStructure.GPIO_Pin = BAT_ADC_SW ;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(BAT_ADC_SW_GPIO, &GPIO_InitStructure );

	//  battery low LED
	//GPIO_InitStructure.GPIO_Pin =BAT_LOW_LED ;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(LED_GPIO, &GPIO_InitStructure );

	// alert LED -- alert gpio is same as dw power gpio i.e portc, we can init them together in one call (below)
	//GPIO_InitStructure.GPIO_Pin = ALERT_LED ;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(LED_GPIO, &GPIO_InitStructure );

	// Buzzer

	//GPIO_InitStructure.GPIO_Pin =  BUZZ;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(BUZZ_GPIO, &GPIO_InitStructure );

	// vibrator
	//GPIO_InitStructure.GPIO_Pin =  VIB;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(VIB_GPIO, &GPIO_InitStructure );

	// dw power on
		GPIO_InitStructure.GPIO_Pin =  DW_PWR | ALERT_LED | BAT_LOW_LED | BAT_ADC_SW;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(DW_GPIO, &GPIO_InitStructure );

	// si sdn pin
		GPIO_InitStructure.GPIO_Pin =  SI_SDN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SI_GPIO, &GPIO_InitStructure );


#else
	//Enable GPIO used for User button
	GPIO_InitStructure.GPIO_Pin = TA_BOOT1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TA_BOOT1_GPIO, &GPIO_InitStructure);

	//Enable GPIO used for Response Delay setting
	GPIO_InitStructure.GPIO_Pin = TA_RESP_DLY | TA_SW1_3 | TA_SW1_4 | TA_SW1_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TA_RESP_DLY_GPIO, &GPIO_InitStructure);

	//Enable GPIO used for SW1 switch setting
	GPIO_InitStructure.GPIO_Pin = TA_SW1_3 | TA_SW1_4 | TA_SW1_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TA_SW1_GPIO, &GPIO_InitStructure);

	// Disable GPIOs clocks
	//RCC_APB2PeriphClockCmd(
	//					RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
	//					RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
	//					RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,
	//					DISABLE);

	// Enable GPIO used for LEDs
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, DISABLE);

#endif

//		GPIO_InitStructure.GPIO_Pin = SI_SCK | SI_MOSI;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//		//GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
//
//		// SPIy MISO pin setup
//		//GPIO_InitStructure.GPIO_Pin = SPIy_MISO;
//		GPIO_InitStructure.GPIO_Pin = SI_MISO;
//		//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
//
//		//GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
//		GPIO_Init(SI_GPIO, &GPIO_InitStructure);
//
//		// SPIy CS pin setup
//		//GPIO_InitStructure.GPIO_Pin = SPIy_CS;
//		GPIO_InitStructure.GPIO_Pin = SI_CS;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//		//GPIO_Init(SPIy_CS_GPIO, &GPIO_InitStructure);
//		GPIO_Init(SI_GPIO, &GPIO_InitStructure);
//

    return 0;
}


void reset_DW1000(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO used for DW1000 reset
	GPIO_InitStructure.GPIO_Pin = DW1000_RSTn;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DW1000_RSTn_GPIO, &GPIO_InitStructure);

	//drive the RSTn pin low
	GPIO_ResetBits(DW1000_RSTn_GPIO, DW1000_RSTn);

	//put the pin back to tri-state ... as input
	GPIO_InitStructure.GPIO_Pin = DW1000_RSTn;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DW1000_RSTn_GPIO, &GPIO_InitStructure);

	Sleep(1);
}
void setup_DW1000RSTnIRQ(int enable)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	if(enable)
	{
		// Enable GPIO used as DECA IRQ for interrupt
		GPIO_InitStructure.GPIO_Pin = DECARSTIRQ;
		//GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_IPD;	//IRQ pin should be Pull Down to prevent unnecessary EXT IRQ while DW1000 goes to sleep mode
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(DECARSTIRQ_GPIO, &GPIO_InitStructure);

		/* Connect EXTI Line to GPIO Pin */
		GPIO_EXTILineConfig(DECARSTIRQ_EXTI_PORT, DECARSTIRQ_EXTI_PIN);

		/* Configure EXTI line */
		EXTI_InitStructure.EXTI_Line = DECARSTIRQ_EXTI;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MP IRQ polarity is high by default
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Set NVIC Grouping to 16 groups of interrupt without sub-grouping */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

		/* Enable and set EXTI Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = DECARSTIRQ_EXTI_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		//put the pin back to tri-state ... as input
		GPIO_InitStructure.GPIO_Pin = DW1000_RSTn;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(DW1000_RSTn_GPIO, &GPIO_InitStructure);

		/* Configure EXTI line */
		EXTI_InitStructure.EXTI_Line = DECARSTIRQ_EXTI;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MP IRQ polarity is high by default
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);
	}
}


int ETH_GPIOConfigure(void)
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ETHERNET pins configuration */
	/* AF Output Push Pull:
	- ETH_MII_MDIO / ETH_RMII_MDIO: PA2
	- ETH_MII_MDC / ETH_RMII_MDC: PC1
	- ETH_MII_TXD2: PC2
	- ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11
	- ETH_MII_TXD0 / ETH_RMII_TXD0: PB12
	- ETH_MII_TXD1 / ETH_RMII_TXD1: PB13
	- ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5
	- ETH_MII_TXD3: PB8 */

	/* Configure PA2 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PC1 and PC2 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 |
								GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/**************************************************************/
	/*               For Remapped Ethernet pins                   */
	/*************************************************************/
	/* Input (Reset Value):
	- ETH_MII_CRS CRS: PA0
	- ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
	- ETH_MII_COL: PA3
	- ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
	- ETH_MII_TX_CLK: PC3
	- ETH_MII_RXD0 / ETH_RMII_RXD0: PD9
	- ETH_MII_RXD1 / ETH_RMII_RXD1: PD10
	- ETH_MII_RXD2: PD11
	- ETH_MII_RXD3: PD12
	- ETH_MII_RX_ER: PB10 */

	/* Configure PA0, PA1 and PA3 as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PB10 as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PC3 as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PD8, PD9, PD10, PD11 and PD12 as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure); /**/



	/* MCO pin configuration------------------------------------------------- */
	/* Configure MCO (PA8) as alternate function push-pull */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

	return 0;
}

int is_button_low(uint16_t GPIOpin)
{
	int result = 1;

	if (GPIO_ReadInputDataBit(TA_BOOT1_GPIO, TA_BOOT1))
		result = 0;

	return result;
}

//when switch (S1) is 'on' the pin is low
int is_switch_on(uint16_t GPIOpin)
{
	int result = 1;

	if (GPIO_ReadInputDataBit(TA_SW1_GPIO, GPIOpin))
		result = 0;

	return result;
}


void led_off (led_t led)
{
	switch (led)
	{
	case LED_PC6:
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		break;
	case LED_PC7:
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		break;
	case LED_PC8:
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		break;
	case LED_PC9:
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		break;
	case LED_ALL:
		GPIO_ResetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7);
		break;
	default:
		// do nothing for undefined led number
		break;
	}
}

void led_on (led_t led)
{
	switch (led)
	{
	case LED_PC6:
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		break;
	case LED_PC7:
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
		break;
	case LED_PC8:
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		break;
	case LED_PC9:
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		break;
	case LED_ALL:
		GPIO_SetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7);
		break;
	default:
		// do nothing for undefined led number
		break;
	}
}



int is_IRQ_enabled(void)
{
	return ((   NVIC->ISER[((uint32_t)(DECAIRQ_EXTI_IRQn) >> 5)]
	           & (uint32_t)0x01 << (DECAIRQ_EXTI_IRQn & (uint8_t)0x1F)  ) ? 1 : 0) ;
}

int peripherals_init (void)
{
	rcc_init();
	gpio_init();
	//rtc_init();
	systick_init();
	timer_init();
	interrupt_init();
	//alertInit();
	usart_init();
	//spi_init();
	ethernet_init();
	//fs_init();
	//usb_init();
	//lcd_init();
	//touch_screen_init();
#if (DMA_ENABLE == 1)
	dma_init();	//init DMA for SPI only. Connection of SPI to DMA in read/write functions
#endif

	return 0;
}

void spi_peripheral_init()
{
	spi_init();

	//SPI2_Configuration();

#ifdef SAFETY_VEST
	//SI_IntConfig(); // configure Si4463 device interface

#else

	//initialise SPI2 peripheral for LCD control
	//SPI2_Configuration();

	//port_LCD_RS_clear();

	//port_LCD_RW_clear();
#endif
}




///////////////////////////////////// ADC peripherals /////////////////////////////////////////////////////////////

#define BAT_CHECK		ADC10

ADC_InitTypeDef BAT_ADC_struct; 	// 	typedef structure for battery and ADC connection
void bat_Init(void)				//	Initialise battery
{

	// initialising battery ADC structure
	BAT_ADC_struct.ADC_Mode = 0x0; // we need only one ADC channel to work independently
	BAT_ADC_struct.ADC_NbrOfChannel = 1; // we need only one ADC channel
	BAT_ADC_struct.ADC_ContinuousConvMode = DISABLE;  // no continuous mode is required
	BAT_ADC_struct.ADC_DataAlign = ADC_DataAlign_Right;  // align ADC output to LSB bits
	BAT_ADC_struct.ADC_ScanConvMode = DISABLE;	// single channel conversation
	BAT_ADC_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// initialising ADC1
	ADC_Init(ADC1, &BAT_ADC_struct);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	bat_adc_on();
	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	bat_adc_off();

}

uint16_t read_BAT_voltage()
{
	uint16_t ADC_val;
	bat_adc_on();
	ADC_RegularChannelConfig(ADC1, BAT_ADC_CHANNEL, 1, ADC_SampleTime_1Cycles5);
	// Start the conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	// Get the conversion value
	bat_adc_off();
	ADC_val = ADC_GetConversionValue(ADC1);
	// battery voltage must be 1.8 => 2233
	if (ADC_val < 2000)
	{	// if battery is low then turn on battery low LED and charge battery
		GPIO_SetBits(LED_GPIO, BAT_LOW_LED);
		GPIO_SetBits(BAT_SHDN_GPIO,BAT_SHDN);
	}
	else if (ADC_val > 2233)
	{	// if battery is full then turn off battery low LED and don't charge battery
		GPIO_ResetBits(LED_GPIO, BAT_LOW_LED);
		GPIO_ResetBits(BAT_SHDN_GPIO,BAT_SHDN);
	}
	else
	{	// otherwise turn off battery low LED and charge battery
		GPIO_ResetBits(LED_GPIO, BAT_LOW_LED);
		GPIO_SetBits(BAT_SHDN_GPIO,BAT_SHDN);
	}

	return ADC_val;
}

void bat_adc_on(void)
{
	GPIO_SetBits(BAT_ADC_SW_GPIO, BAT_ADC_SW);
}

void bat_adc_off(void)
{
	GPIO_ResetBits(BAT_ADC_SW_GPIO, BAT_ADC_SW);
}


//////////////////////////////////// END ADC peripherals //////////////////////////////////////////////////////////////

///////////////////////////////////// Si4463 peripherals /////////////////////////////////////////////////////////////
void SI_IntConfig()
{

	#ifndef SAFETY_VEST
		#undef SPIy // undefined SPIy because it is same as SI_SPI
		#undef SPIy_GPIO
		#undef SPIy_CS
		#undef SPIy_CS_GPIO
		#undef SPIy_SCK
		#undef SPIy_MISO
		#undef SPIy_MOSI


	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

// 1.============= Initialise GPIO =======================================
	// SI_SPI SCK and MOSI pin setup
	GPIO_InitStructure.GPIO_Pin = SI_SCK | SI_MOSI | SI_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(SI_GPIO, &GPIO_InitStructure);

	// SI_SPI MISO pin setup
	GPIO_InitStructure.GPIO_Pin = SI_MISO;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

	GPIO_Init(SI_GPIO, &GPIO_InitStructure);

	// Disable SI_SPI SS Output
	SPI_SSOutputCmd(SI_SPI, DISABLE);
	// Set CS high
	GPIO_SetBits(SI_GPIO, SI_CS);

// 2.============= initialise SPI =======================================================
	SPI_I2S_DeInit(SI_SPI);
	// SI_SPI Mode setup
	//SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	 //
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SI_BautRatePrescaler;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SI_SPI, &SPI_InitStructure);
	SPI_Cmd(SI_SPI, ENABLE);

	//TODO: conifgure si interrupts

#endif
}


///////////////////////			timers 		/////////////////////////////////////////////////////////////////////////

void Timer_Configuration()
{
	TIM_TimeBaseInitTypeDef timerInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    timerInitStructure.TIM_Prescaler = 12000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 0xffff;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, DISABLE);


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    timerInitStructure.TIM_Prescaler = 12;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    timerInitStructure.TIM_Period = 0xffff;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &timerInitStructure);
    TIM_Cmd(TIM3, ENABLE);
}

void delay_ms(uint delay_ms)
{
	TIM2->PSC = TICKS_PER_MS;
	TIM2->ARR = delay_ms*10;
	TIM2->CNT = 0;
	TIM2->EGR = TIM_EGR_UG;  // copy values into actual registers!
							 // Enable timer in one pulse mode
	TIM2->CR1 |= (TIM_CR1_OPM | TIM_CR1_CEN);
	while (TIM2->CR1 & TIM_CR1_CEN); // wait for it to switch off

}
////////////////////////		timers end  //////////////////////////////////////////////////////////////



// =================================== delay for RF4463 =================================
void Delay(u32 timecounts)
{
  u32 i;
  for(i = 0 ; i < timecounts ; ++i){;}
}

void ADC_Configuration(void)
{
  ADC_InitTypeDef  ADC_InitStructure;
  /* PCLK2 is the APB2 clock */
  /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* Enable ADC1 clock so that we can talk to it */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* Put everything back to power-on defaults */
  ADC_DeInit(ADC1);

  /* ADC1 Configuration ------------------------------------------------------*/
  /* ADC1 and ADC2 operate independently */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  /* Disable the scan conversion so we do one at a time */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  /* Don't do contimuous conversions - do them on demand */
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  /* Start conversin by software, not an external trigger */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  /* Conversions are 12 bit - put them in the lower 12 bits of the result */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  /* Say how many channels would be used by the sequencer */
  ADC_InitStructure.ADC_NbrOfChannel = 1;

  /* Now do the setup */
  ADC_Init(ADC1, &ADC_InitStructure);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

uint16 readADC1(uint8 channel)
{
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
  // Start the conversion
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  return ADC_GetConversionValue(ADC1);
}
