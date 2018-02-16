/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
//#define __CHECK_DEVICE_DEFINE

#include "stm32f0xx.h"
volatile uint32_t millis=0;
#include "board.h"
#include "si5351.h"

int main(void)
{
	Si5351_ConfigTypeDef Si5351_ConfigStruct;
	uint8_t tmp;
	uint32_t i;

	//initialize the board
	Init_Board();

	//intialize configuration structure
	Si5351_StructInit(&Si5351_ConfigStruct);

	//set some basic configuration stuff
	//PLL multiplier 32x = 800 MHz
	Si5351_ConfigStruct.PLL[0].PLL_Multiplier_Integer = 32;

	//divide PLL by 8 = 100 MHz
	Si5351_ConfigStruct.MS[0].MS_Divider_Integer = 8;

	//divide the Multisynth output by 4 = 25 MHz
	Si5351_ConfigStruct.CLK[0].CLK_R_Div = CLK_R_Div4;

	//enable output
	Si5351_ConfigStruct.CLK[0].CLK_Enable = ON;

	//do not enable Spread Spectrum
	Si5351_ConfigStruct.SS.SS_Amplitude_ppm = 30000;
	Si5351_ConfigStruct.SS.SS_Enable = OFF;

	//initialize the Si5351
	Si5351_Init(&Si5351_ConfigStruct);

	while (0) {} //just hang

	while (0) //periodically change PLL multiplier
	{
		for (i=8; i<= 38; i++)
		{
			Delay_ms(2000);
			Si5351_ConfigStruct.PLL[0].PLL_Multiplier_Integer = i;
			Si5351_PLLConfig(&Si5351_ConfigStruct, PLL_A);
			//Si5351_PLLReset(&Si5351_ConfigStruct, PLL_A);
		}
	}

	Si5351_ConfigStruct.MS[0].MS_Divider_Denominator = 12;
	while (1) { //periodically change output frequency
		for (i = 0; i <= 100; i++)
		{
			Si5351_ConfigStruct.MS[0].MS_Divider_Numerator = i;
			//Si5351_PLLConfig(&Si5351_ConfigStruct, PLL_A);
			Si5351_MSConfig(&Si5351_ConfigStruct, MS0);
			Delay_ms(50);
		}
		for (i = 100; i >= 1; i--)
		{
			Si5351_ConfigStruct.MS[0].MS_Divider_Numerator = i;
			//Si5351_PLLConfig(&Si5351_ConfigStruct, PLL_A);
			Si5351_MSConfig(&Si5351_ConfigStruct, MS0);
			Delay_ms(50);
		}

		Si5351_ConfigStruct.MS[0].MS_Divider_Numerator = 0;
		Si5351_MSConfig(&Si5351_ConfigStruct, MS0);
	}

	while(1)
	{ //indicate error flags
		if (Si5351_CheckStatusBit(&Si5351_ConfigStruct, StatusBit_PLLA))
		{
			GPIO_WriteBit(GPIOA, LED_OK,   Bit_SET);
		} else {
			GPIO_WriteBit(GPIOA, LED_OK,   Bit_RESET); //turn on LED if PLL OK
		}

		if (Si5351_CheckStatusBit(&Si5351_ConfigStruct, StatusBit_SysInit))
		{
			GPIO_WriteBit(GPIOA, LED_UP,   Bit_SET);
		} else {
			GPIO_WriteBit(GPIOA, LED_UP,   Bit_RESET); //turn on LED if PLL OK
		}

		if (Si5351_CheckStatusBit(&Si5351_ConfigStruct, StatusBit_PLLB))
		{
			GPIO_WriteBit(GPIOA, LED_DOWN,   Bit_SET);
		} else {
			GPIO_WriteBit(GPIOA, LED_DOWN,   Bit_RESET); //turn on LED if PLL OK
		}

		if (Si5351_CheckStatusBit(&Si5351_ConfigStruct, StatusBit_CLKIN))
		{
			GPIO_WriteBit(GPIOA, LED_RIGHT,   Bit_SET);
		} else {
			GPIO_WriteBit(GPIOA, LED_RIGHT,   Bit_RESET); //turn on LED if PLL OK
		}
	}
}
