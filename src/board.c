/*
 * board.c
 *
 *  Created on: Dec 24, 2017
 *      Author: msboss
 */
#include <stm32f0xx_rcc.h>
#include "board.h"
#include <stm32f0xx_misc.h>

typedef enum {FALSE=0, TRUE=1} bool;
bool I2C_Initialised = FALSE;
extern volatile uint32_t millis;

void SysTick_Handler(void)
{
	millis++;
}

void Delay_ms(uint32_t Wait)
{
	uint32_t temp=millis;
	while((millis-temp) < Wait) asm volatile ("nop");
}

void Init_Board(void)
{
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);	//select HSI as SYSCLK source during setup
	RCC_HSEConfig(RCC_HSE_ON);				//enable HSE
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY)==RESET) asm volatile ("nop");
		//wait for HSE to get ready
	RCC_ClockSecuritySystemCmd(ENABLE);		//enable CSS
	RCC_PREDIV1Config(RCC_PREDIV1_Div2);	//divide HSE by 2 as input to PLL
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_12); //select HSE/2 as PLL source, multiply to 6*HSE
	RCC_PLLCmd(ENABLE);						//enable PLL
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET) asm volatile ("nop");
		//wait for PLL to get ready
	RCC_HCLKConfig(RCC_SYSCLK_Div1);		//set HCLK to SYSCLK/1
	RCC_PCLKConfig(RCC_HCLK_Div1);			//set PCLK to HCLK/1 (=SYSCLK)
	RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);		//set I2C clock to HSI
//	RCC_ADCCLKConfig(RCC_ADCCLK_HSI14);		//set ADC clock to HSI14 (obsolete)
//	ADC_ClockModeConfig(1, ADC_ClockMode_AsynClk);	//set ADC clock to HSI14
 	RCC_USARTCLKConfig(RCC_USART1CLK_SYSCLK);		//set USART1 clock to SYSCLK
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		//set SYSCLK to PLLCLK
	while(RCC_GetSYSCLKSource() != 0x08) asm volatile ("nop");
		//wait for PLL to be set as SYSCLK

	//current settings: SYSCLK=HCLK=PCLK=USART1CLK= 12*HSE/2 = 6*HSE = 48 MHZ
	//					I2C1CLK=HSI
	//					ADCCLK=HSI14
	//					CSS ON - backs up SYSCLK with HSI if HSE fails
	//TODO: add RTC clock select

	SystemCoreClockUpdate();
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_Config(SystemCoreClock/1000);

	Button_Init();
	I2C_Board_Init(ENABLE);
}

void I2C_Board_Init(ForceState Force_Init)
{
	GPIO_InitTypeDef GPIO_Initstruct;
	I2C_InitTypeDef I2C_InitStruct;

	if((I2C_Initialised==FALSE) | (Force_Init==ENABLED))
	{
		I2C_Initialised=TRUE;

		GPIO_Initstruct.GPIO_Pin = SDA | SCL;
		GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Initstruct.GPIO_OType = GPIO_OType_OD;
		GPIO_Initstruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Initstruct.GPIO_Speed = GPIO_Speed_10MHz;

		I2C_StructInit(&I2C_InitStruct);
		I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
		I2C_InitStruct.I2C_DigitalFilter = 0;
		//TODO: set sensible digital constant
		I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStruct.I2C_OwnAddress1 = 0x039;
		//TODO: set corrrect timing
		I2C_InitStruct.I2C_Timing = 0x00901850;
		//I2C_InitStruct.I2C_Timing = 0x10805e89;
		//TODO: set sensible timing constant

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  DISABLE);
				//reset I2C1

		RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //enable GPIOA clock
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  ENABLE);
		//RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,  ENABLE);
		//RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  ENABLE);
		GPIO_Init(GPIOA, &GPIO_Initstruct);
		//init I2C pins
		//GPIO_PinAFConfig(GPIOA, (uint32_t)(1 << (SDA & 0x1F)), GPIO_AF_4);
		//GPIO_PinAFConfig(GPIOA, (uint32_t)(1 << (SCL & 0x1F)), GPIO_AF_4);
		GPIO_PinAFConfig(GPIOA, SDA_PinSource, GPIO_AF_4);
		GPIO_PinAFConfig(GPIOA, SCL_PinSource, GPIO_AF_4);

		I2C_Init(I2C1, &I2C_InitStruct);
				//initialize I2C
		I2C_Cmd(I2C1, ENABLE);
	}
}

void I2C_Board_Deinit(ForceState Force_Deinit)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);

	if((I2C_Initialised==TRUE) | (Force_Deinit==ENABLED))
	{
		I2C_Initialised=FALSE;
		I2C_Cmd(I2C1, DISABLE);
		I2C_DeInit(I2C1);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Pin = SCL | SDA ;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void Button_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_StructInit(&GPIO_InitStruct);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //enable GPIOA clock

	GPIO_InitStruct.GPIO_Pin = LED_BACK | LED_OK | LED_UP | LED_DOWN |
			LED_LEFT | LED_RIGHT;
			//pins PA0-PA6
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
			//set them to open-drain outputs with pull-up on low speed

	GPIO_WriteBit(GPIOA, LED_BACK, Bit_RESET);
	GPIO_WriteBit(GPIOA, LED_OK,   Bit_SET);
	GPIO_WriteBit(GPIOA, LED_UP,   Bit_SET);
	GPIO_WriteBit(GPIOA, LED_DOWN, Bit_SET);
	GPIO_WriteBit(GPIOA, LED_LEFT, Bit_SET);
	GPIO_WriteBit(GPIOA, LED_RIGHT,Bit_SET);
			//turn off all LEDs except RED back/cancel diode
}

void Button_Deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = LED_BACK | LED_OK | LED_UP | LED_DOWN |
			LED_LEFT | LED_RIGHT;
			//pins PA0-PA6
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
			//set them to open-drain outputs with pull-up on low speed
}
