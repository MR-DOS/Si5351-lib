/*
 * si5351.c
 *
 *  Created on: Jan 14, 2019
 *      Author: Petr Polasek
 *
 *      To make this library useable on any other device than
 *      STM32Fxxx Cortex Mx, please edit these parts of the library:
 *
 *      DEFINES:
 *      SI5351_I2C_PERIPHERAL - the I2C peripheral name according
 *      	to your devices HAL library
 *      I2C_TIMEOUT - time for the communication to time out
 *
 *      TYPEDEFS:
 *      Si5351_ConfigTypeDef - the I2Cx parameter should be changed
 *      	so that its type corresponds to your HAL library
 *
 *      FUNCTIONS:
 *      Si5351_WriteRegister
 *      Si5351_ReadRegister
 *      	You need to write your own I2C handlers here
 *
 */

//put your I2C HAL library name here
#include "stm32f10x_i2c.h"

#include "si5351.h"

int Si5351_WriteRegister(Si5351_ConfigTypeDef *Si5351_ConfigStruct,  uint8_t reg_address, uint8_t reg_data)
{
	uint32_t error_wait;

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(Si5351_ConfigStruct->I2Cx, I2C_FLAG_BUSY) == SET)
	{
		error_wait--;
		if (error_wait==0)
		{
			I2C_SoftwareResetCmd(Si5351_ConfigStruct->I2Cx, ENABLE);
			I2C_SoftwareResetCmd(Si5351_ConfigStruct->I2Cx, DISABLE);
			return 1;
		}
	}
	//wait for I2C to get ready, if not ready in time, reset I2C and return

	I2C_GenerateSTART(Si5351_ConfigStruct->I2Cx, ENABLE);
	//send START condition

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for START to be sent, if not sent in time, return

	I2C_Send7bitAddress(Si5351_ConfigStruct->I2Cx, Si5351_ConfigStruct->HW_I2C_Address, I2C_Direction_Transmitter);
	//send address+RW bit

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for address to be sent, if not sent in time, return

	I2C_SendData(Si5351_ConfigStruct->I2Cx, reg_address);
	//send reg address

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for reg address to be sent

	I2C_SendData(Si5351_ConfigStruct->I2Cx, reg_data);
	//send reg data

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for data to be sent, if not sent in time, return

	I2C_GenerateSTOP(Si5351_ConfigStruct->I2Cx, ENABLE);
	//generate STOP condition

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(Si5351_ConfigStruct->I2Cx, I2C_FLAG_STOPF))
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait until STOP is cleared

	return 0;
}

uint8_t Si5351_ReadRegister(Si5351_ConfigTypeDef *Si5351_ConfigStruct,  uint8_t reg_address)
{
	uint32_t error_wait;

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(Si5351_ConfigStruct->I2Cx, I2C_FLAG_BUSY) == SET)
	{
		error_wait--;
		if (error_wait==0)
		{
			I2C_SoftwareResetCmd(Si5351_ConfigStruct->I2Cx, ENABLE);
			I2C_SoftwareResetCmd(Si5351_ConfigStruct->I2Cx, DISABLE);
			return 1;
		}
	}
	//wait for I2C to get ready, if not ready in time, reset I2C and return

	I2C_GenerateSTART(Si5351_ConfigStruct->I2Cx, ENABLE);
	//send START condition

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for START to be sent, if not sent in time, return

	I2C_Send7bitAddress(Si5351_ConfigStruct->I2Cx, Si5351_ConfigStruct->HW_I2C_Address, I2C_Direction_Transmitter);
	//send address+RW bit

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for address to be sent, if not sent in time, return

	I2C_SendData(Si5351_ConfigStruct->I2Cx, reg_address);
	//send reg address

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for reg address to be sent

	I2C_GenerateSTOP(Si5351_ConfigStruct->I2Cx, ENABLE);
	//generate STOP condition

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(Si5351_ConfigStruct->I2Cx, I2C_FLAG_STOPF))
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait until STOP is cleared


	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(Si5351_ConfigStruct->I2Cx, I2C_FLAG_BUSY) == SET)
	{
		error_wait--;
		if (error_wait==0)
		{
			I2C_SoftwareResetCmd(Si5351_ConfigStruct->I2Cx, ENABLE);
			I2C_SoftwareResetCmd(Si5351_ConfigStruct->I2Cx, DISABLE);
			return 1;
		}
	}
	//wait for I2C to get ready, if not ready in time, reset I2C and return

	I2C_GenerateSTART(Si5351_ConfigStruct->I2Cx, ENABLE);
	//send START condition

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for START to be sent, if not sent in time, return

	I2C_Send7bitAddress(Si5351_ConfigStruct->I2Cx, Si5351_ConfigStruct->HW_I2C_Address, I2C_Direction_Receiver);
	//send address+RW bit

	error_wait = I2C_TIMEOUT;
	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for address to be sent, if not sent in time, return

	while (I2C_CheckEvent(Si5351_ConfigStruct->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for data

	uint8_t reg_data;
	reg_data = I2C_ReceiveData(Si5351_ConfigStruct->I2Cx);
	//receive reg data

	I2C_GenerateSTOP(Si5351_ConfigStruct->I2Cx, ENABLE);
	//generate STOP condition

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(Si5351_ConfigStruct->I2Cx, I2C_FLAG_STOPF))
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait until STOP is cleared

	return reg_data;
}

//set safe values in the config structure
void Si5351_StructInit(Si5351_ConfigTypeDef *Si5351_ConfigStruct)
{
	uint8_t i;

	Si5351_ConfigStruct->HW_I2C_Address = SI5351_I2C_ADDRESS;
	Si5351_ConfigStruct->I2Cx = SI5351_I2C_PERIPHERAL;

	Si5351_ConfigStruct->f_CLKIN = SI5351_CLKIN_FREQ;
	Si5351_ConfigStruct->f_XTAL = SI5351_XTAL_FREQ;

	Si5351_ConfigStruct->Interrupt_Mask_CLKIN = ON;
	Si5351_ConfigStruct->Interrupt_Mask_PLLA = ON;
	Si5351_ConfigStruct->Interrupt_Mask_PLLB = ON;
	Si5351_ConfigStruct->Interrupt_Mask_SysInit = ON;
	Si5351_ConfigStruct->Interrupt_Mask_XTAL = ON;

	Si5351_ConfigStruct->Fanout_CLKIN_EN = ON;
	Si5351_ConfigStruct->Fanout_MS_EN = ON;
	Si5351_ConfigStruct->Fanout_XO_EN = ON;

	Si5351_ConfigStruct->OSC.CLKIN_Div = CLKINDiv_Div1;
	Si5351_ConfigStruct->OSC.OSC_XTAL_Load = XTAL_Load_10_pF;
	Si5351_ConfigStruct->OSC.VCXO_Pull_Range_ppm = 0; //maybe should be set to 30 ppm, not clear from the AN-619

	for (i=0; i<=1; i++)
	{
		Si5351_ConfigStruct->PLL[i].PLL_Clock_Source = PLL_Clock_Source_XTAL;
		Si5351_ConfigStruct->PLL[i].PLL_Multiplier_Integer = 32; 		//range 24..36 for 25 MHz clock
		Si5351_ConfigStruct->PLL[i].PLL_Multiplier_Numerator = 0; 		//range 0..1048575
		Si5351_ConfigStruct->PLL[i].PLL_Multiplier_Denominator = 1; 	//range 1..1048575
		Si5351_ConfigStruct->PLL[i].PLL_Capacitive_Load = PLL_Capacitive_Load_0;		//select 0, unless you want to tune the PLL to <200 MHZ
	}

	Si5351_ConfigStruct->SS.SS_Amplitude_ppm = 0; //1.5% modulation = 15000
	Si5351_ConfigStruct->SS.SS_Enable = OFF;
	Si5351_ConfigStruct->SS.SS_Mode = SS_Mode_CenterSpread;
	Si5351_ConfigStruct->SS.SS_NCLK = SS_NCLK_0; //default value, this parameter is unexplained in documentation

	for (i=0; i<=7; i++)
	{
		Si5351_ConfigStruct->MS[i].MS_Clock_Source = MS_Clock_Source_PLLA;
		Si5351_ConfigStruct->MS[i].MS_Divider_Integer = 4;
		Si5351_ConfigStruct->MS[i].MS_Divider_Numerator = 0;
		Si5351_ConfigStruct->MS[i].MS_Divider_Denominator = 1;

		Si5351_ConfigStruct->CLK[i].CLK_Clock_Source = CLK_Clock_Source_MS_Own;
		Si5351_ConfigStruct->CLK[i].CLK_Disable_State = CLK_Disable_State_HIGH_Z;
		Si5351_ConfigStruct->CLK[i].CLK_Enable = OFF;
		Si5351_ConfigStruct->CLK[i].CLK_I_Drv = CLK_I_Drv_8mA;
		Si5351_ConfigStruct->CLK[i].CLK_Invert = OFF;
		Si5351_ConfigStruct->CLK[i].CLK_PowerDown = OFF;
		Si5351_ConfigStruct->CLK[i].CLK_QuarterPeriod_Offset = 0;
		Si5351_ConfigStruct->CLK[i].CLK_R_Div = CLK_R_Div1;
		Si5351_ConfigStruct->CLK[i].CLK_Use_OEB_Pin = OFF;
	}
}

void Si5351_OSCConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct)
{
	uint8_t tmp;
	uint32_t VCXO_Param;

	//set XTAL capacitive load and PLL VCO load capacitance
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_XTAL_CL);
	tmp &= ~(XTAL_CL_MASK | PLL_CL_MASK);
	tmp |= (XTAL_CL_MASK & (Si5351_ConfigStruct->OSC.OSC_XTAL_Load)) | (PLL_CL_MASK & ((Si5351_ConfigStruct->PLL[0].PLL_Capacitive_Load) << 1)) | (PLL_CL_MASK & ((Si5351_ConfigStruct->PLL[1].PLL_Capacitive_Load) << 4));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_XTAL_CL, tmp);

	//set CLKIN pre-divider
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLKIN_DIV);
	tmp &= ~CLKIN_MASK;
	tmp |= CLKIN_MASK & Si5351_ConfigStruct->OSC.CLKIN_Div;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLKIN_DIV, tmp);

	//set fanout of XO, MS0, MS4 and CLKIN - should be always on unless you
	//need to reduce power consumption
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_FANOUT_EN);
	tmp &= ~(FANOUT_CLKIN_EN_MASK | FANOUT_MS_EN_MASK | FANOUT_XO_EN_MASK);
	if (Si5351_ConfigStruct->Fanout_CLKIN_EN == ON) tmp |= FANOUT_CLKIN_EN_MASK;
	if (Si5351_ConfigStruct->Fanout_MS_EN == ON) tmp |= FANOUT_MS_EN_MASK;
	if (Si5351_ConfigStruct->Fanout_XO_EN == ON) tmp |= FANOUT_XO_EN_MASK;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_FANOUT_EN, tmp);

	//if "b" in PLLB set to 10^6, set VCXO parameter
	if (Si5351_ConfigStruct->PLL[1].PLL_Multiplier_Denominator == 1000000)
	{
		VCXO_Param = VCXO_PARAM_MASK & (uint32_t)
				((103 * Si5351_ConfigStruct->OSC.VCXO_Pull_Range_ppm
						* ((uint64_t)128000000 * Si5351_ConfigStruct->PLL[1].PLL_Multiplier_Integer +
								Si5351_ConfigStruct->PLL[1].PLL_Multiplier_Numerator))/100000000);
	} else {
		VCXO_Param = 0;
	}

	tmp = (uint8_t) VCXO_Param;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_VCXO_PARAM_0_7, tmp);
	tmp = (uint8_t)(VCXO_Param>>8);
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_VCXO_PARAM_8_15, tmp);
	tmp = (uint8_t)((VCXO_Param>>16) & VCXO_PARAM_16_21_MASK);
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_VCXO_PARAM_16_21, tmp);
}

EnableState Si5351_CheckStatusBit(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_StatusBitTypeDef StatusBit)
{
	uint8_t tmp;

	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_DEV_STATUS);
	tmp &= StatusBit;
	return tmp;
}

EnableState Si5351_CheckStickyBit(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_StatusBitTypeDef StatusBit)
{
	uint8_t tmp;

	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_DEV_STICKY);
	tmp &= StatusBit;
	return tmp;
}

void Si5351_InterruptConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct)
{
	uint8_t tmp;
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_INT_MASK);

	tmp &= ~INT_MASK_LOS_XTAL_MASK;
	if (Si5351_ConfigStruct->Interrupt_Mask_XTAL == ON)
	{
		tmp |= INT_MASK_LOS_XTAL_MASK;
	}

	tmp &= ~INT_MASK_LOS_CLKIN_MASK;
	if (Si5351_ConfigStruct->Interrupt_Mask_CLKIN == ON)
	{
		tmp |= INT_MASK_LOS_CLKIN_MASK;
	}

	tmp &= ~INT_MASK_LOL_A_MASK;
	if (Si5351_ConfigStruct->Interrupt_Mask_PLLA == ON)
	{
		tmp |= INT_MASK_LOL_A_MASK;
	}

	tmp &= ~INT_MASK_LOL_B_MASK;
	if (Si5351_ConfigStruct->Interrupt_Mask_PLLB == ON)
	{
		tmp |= INT_MASK_LOL_B_MASK;
	}

	tmp &= ~INT_MASK_SYS_INIT_MASK;
	if (Si5351_ConfigStruct->Interrupt_Mask_SysInit == ON)
	{
		tmp |= INT_MASK_SYS_INIT_MASK;
	}

	Si5351_WriteRegister(Si5351_ConfigStruct, REG_INT_MASK, tmp);
}

void Si5351_ClearStickyBit(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_StatusBitTypeDef StatusBit)
{
	uint8_t tmp;

	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_DEV_STICKY);
	tmp &= ~StatusBit;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_DEV_STICKY, tmp);
}

void Si5351_PLLConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_PLLChannelTypeDef PLL_Channel)
{
	uint8_t tmp, tmp_mask;
	uint32_t MSN_P1, MSN_P2, MSN_P3;

	//set PLL clock source
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_PLL_CLOCK_SOURCE);
	tmp_mask = PLLA_CLOCK_SOURCE_MASK << PLL_Channel;
	tmp &= ~tmp_mask;
	tmp |= tmp_mask & Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Clock_Source;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_PLL_CLOCK_SOURCE, tmp);

	//if new multiplier not even  integer, disable the integer mode
	if ((Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Numerator != 0) | ((Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Integer & 0x01) != 0 ))
	{
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_FB_INT + PLL_Channel);
		tmp &= ~FB_INT_MASK;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_FB_INT + PLL_Channel, tmp);
	}

	//configure the PLL multiplier
	MSN_P1 = 128 * Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Integer + ((128 * Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Numerator) / Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Denominator) - 512;
	MSN_P2 = 128 * Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Numerator - Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Denominator * ((128 * Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Numerator) / Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Denominator);
	MSN_P3 = Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Denominator;

	tmp = (uint8_t) MSN_P1;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P1_0_7 + 8 * PLL_Channel, tmp);
	tmp = (uint8_t) (MSN_P1 >> 8);
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P1_8_15 + 8 * PLL_Channel, tmp);
	tmp = (uint8_t) (MSN_P1_16_17_MASK & (MSN_P1 >> 16));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P1_16_17 + 8 * PLL_Channel, tmp);

	tmp = (uint8_t) MSN_P2;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P2_0_7 + 8 * PLL_Channel, tmp);
	tmp = (uint8_t) (MSN_P2 >> 8);
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P2_8_15 + 8 * PLL_Channel, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MSN_P2_16_19);
	tmp &= ~MSN_P2_16_19_MASK;
	tmp |= (uint8_t) (MSN_P2_16_19_MASK & (MSN_P2 >> 16));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P2_16_19 + 8 * PLL_Channel, tmp);

	tmp = (uint8_t) MSN_P3;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P3_0_7 + 8 * PLL_Channel, tmp);
	tmp = (uint8_t) (MSN_P3 >> 8);
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P3_8_15 + 8 * PLL_Channel, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MSN_P3_16_19);
	tmp &= ~MSN_P3_16_19_MASK;
	tmp |= (uint8_t) (MSN_P3_16_19_MASK & ((MSN_P3 >> 16) << 4));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MSN_P3_16_19 + 8 * PLL_Channel, tmp);

	//if new multiplier is an even integer, enable integer mode
	if ((Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Numerator == 0) & ((Si5351_ConfigStruct->PLL[PLL_Channel].PLL_Multiplier_Integer & 0x01) == 0 ))
	{
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_FB_INT + PLL_Channel);
		tmp |= FB_INT_MASK;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_FB_INT + PLL_Channel, tmp);
	}
}

void Si5351_PLLReset(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_PLLChannelTypeDef PLL_Channel)
{
	uint8_t tmp;

	//reset PLL
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_PLL_RESET);
	if (PLL_Channel == PLL_A)
	{
		tmp |= PLLA_RESET_MASK;
	} else {
		tmp |= PLLB_RESET_MASK;
	}
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_PLL_RESET, tmp);
}

void Si5351_PLLSimultaneousReset(Si5351_ConfigTypeDef *Si5351_ConfigStruct)
{
	uint8_t tmp;

	//reset PLL
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_PLL_RESET);
	tmp |= PLLA_RESET_MASK | PLLB_RESET_MASK;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_PLL_RESET, tmp);
}

void Si5351_SSConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct)
{
	uint8_t tmp;
	uint32_t SSUDP, SSUP_P1, SSUP_P2, SSUP_P3, SSDN_P1, SSDN_P2, SSDN_P3;
	uint64_t SSDN, SSUP;

	//turn off SS if it should be disabled
	if ((Si5351_ConfigStruct->SS.SS_Enable == OFF)|
			(((Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Integer & 0x01) == 0)
					& (Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Numerator == 0)) )
	{
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSC_EN);
		tmp &= ~SSC_EN_MASK;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSC_EN, tmp);
	}

	//set default value of SS_NCLK - spread spectrum reserved register
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SS_NCLK);
	tmp &= ~SS_NCLK_MASK;
	tmp |= SS_NCLK_MASK & (Si5351_ConfigStruct->SS.SS_NCLK);
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SS_NCLK, tmp);

	//set SS mode
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSC_MODE);
	tmp &= ~SSC_MODE_MASK;
	tmp |= SSC_MODE_MASK & Si5351_ConfigStruct->SS.SS_Mode;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSC_MODE, tmp);

	//set SSUDP parameter
	if (Si5351_ConfigStruct->PLL[0].PLL_Clock_Source == PLL_Clock_Source_CLKIN)
	{
		SSUDP = (Si5351_ConfigStruct->f_CLKIN)/(4*31500);
	} else {
		SSUDP = (Si5351_ConfigStruct->f_XTAL)/(4*31500);
	}

	//set SSUDP parameter
	tmp = (uint8_t) SSUDP;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUDP_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSUDP_8_11);
	tmp &= ~SSUDP_8_11_MASK;
	tmp |= (uint8_t) (SSUDP_8_11_MASK & ((SSUDP >> 8) << 4));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUDP_8_11, tmp);

	//calculate SSUP and SSDN parameters
	if (Si5351_ConfigStruct->SS.SS_Mode == SS_Mode_CenterSpread)
	{
		SSUP = ((uint64_t)(64000000*Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Integer
			 	  + (64000000*Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Numerator)/(Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Denominator)
			 	 ) * Si5351_ConfigStruct->SS.SS_Amplitude_ppm
				) / ((1000000 - Si5351_ConfigStruct->SS.SS_Amplitude_ppm) * SSUDP);

		SSDN = ((uint64_t)(64000000*Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Integer
			 	  + (64000000*Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Numerator)/(Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Denominator)
			 	 ) * Si5351_ConfigStruct->SS.SS_Amplitude_ppm
				) / ((1000000 + Si5351_ConfigStruct->SS.SS_Amplitude_ppm) * SSUDP);

		SSUP_P1 = (uint32_t) (SSUP/1000000);
		SSUP_P2 = (uint32_t)(32767*(SSUP/1000000-SSUP_P1));
		SSUP_P3 = 0x7FFF;

	} else {

		SSDN = ((uint64_t)(64000000*Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Integer
				 	 + (64000000*Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Numerator)/(Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Denominator)
				 ) * Si5351_ConfigStruct->SS.SS_Amplitude_ppm
				) / ((1000000 + Si5351_ConfigStruct->SS.SS_Amplitude_ppm) * SSUDP);

		SSUP_P1 = 0;
		SSUP_P2 = 0;
		SSUP_P3 = 1;

	}

	//set SSDN parameter
	SSDN_P1 = (uint32_t) (SSDN/1000000);
	SSDN_P2 = (uint32_t)(32767*(SSDN/1000000-SSDN_P1));
	SSDN_P3 = 0x7FFF;

	//write SSUP parameter P1
	tmp = (uint8_t) SSUP_P1;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUP_P1_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSUP_P1_8_11);
	tmp &= ~SSUP_P1_8_11_MASK;
	tmp |= (uint8_t)(SSUP_P1_8_11_MASK & (SSUP_P1 >> 8));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUP_P1_8_11, tmp);

	//write SSUP parameter P2
	tmp = (uint8_t) SSUP_P2;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUP_P2_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSUP_P2_8_14);
	tmp &= ~SSUP_P2_8_14_MASK;
	tmp |= (uint8_t)(SSUP_P2_8_14_MASK & (SSUP_P2 >> 8));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUP_P2_8_14, tmp);

	//write SSUP parameter P3
	tmp = (uint8_t) SSUP_P3;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUP_P3_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSUP_P3_8_14);
	tmp &= ~SSUP_P3_8_14_MASK;
	tmp |= (uint8_t)(SSUP_P3_8_14_MASK & (SSUP_P3 >> 8));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSUP_P3_8_14, tmp);

	//write SSDN parameter P1
	tmp = (uint8_t) SSDN_P1;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSDN_P1_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSDN_P1_8_11);
	tmp &= ~SSDN_P1_8_11_MASK;
	tmp |= (uint8_t)(SSDN_P1_8_11_MASK & (SSDN_P1 >> 8));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSDN_P1_8_11, tmp);

	//write SSDN parameter P2
	tmp = (uint8_t) SSDN_P2;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSDN_P2_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSDN_P2_8_14);
	tmp &= ~SSDN_P2_8_14_MASK;
	tmp |= (uint8_t)(SSDN_P2_8_14_MASK & (SSDN_P2 >> 8));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSDN_P2_8_14, tmp);

	//write SSDN parameter P3
	tmp = (uint8_t) SSDN_P3;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSDN_P3_0_7, tmp);
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSDN_P3_8_14);
	tmp &= ~SSDN_P3_8_14_MASK;
	tmp |= (uint8_t)(SSDN_P3_8_14_MASK & (SSDN_P3 >> 8));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSDN_P3_8_14, tmp);

	//turn on SS if it should be enabled
	if ((Si5351_ConfigStruct->SS.SS_Enable == ON)
			& (((Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Integer & 0x01) != 0)
					| (Si5351_ConfigStruct->PLL[0].PLL_Multiplier_Numerator != 0)))
	{
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_SSC_EN);
		tmp |= SSC_EN_MASK;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_SSC_EN, tmp);
	}
}

void Si5351_MSConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_MSChannelTypeDef MS_Channel)
{
	uint8_t tmp;
	uint32_t MS_P1, MS_P2, MS_P3;

	//configure MultiSynth clock source
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_SRC + MS_Channel);
	tmp &= ~MS_SRC_MASK;
	if (Si5351_ConfigStruct->MS[MS_Channel].MS_Clock_Source == MS_Clock_Source_PLLB)
	{
		tmp |= MS_SRC_MASK;
	}
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_SRC + MS_Channel, tmp);

	if (MS_Channel <= MS5) //configuration is simpler for MS6 and 7 since they are integer-only
	{
		//if next value not in even integer mode or if divider is not equal to 4, disable DIVBY4
		if ((Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Integer != 4)|(Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Numerator != 0))
		{
			tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_DIVBY4 + 8 * MS_Channel);
			tmp &= ~MS_DIVBY4_MASK;
			Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_DIVBY4 + 8 * MS_Channel, tmp);
		}

		//if next value not in even integer mode or SS enabled, disable integer mode
		if ((Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Numerator != 0)|((Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Integer & 0x01) != 0)|(Si5351_ConfigStruct->SS.SS_Enable == ON))
		{
			tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_INT + MS_Channel);
			tmp &= ~MS_INT_MASK;
			Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_INT + MS_Channel, tmp);
		}

		//set new divider value
		MS_P1 = 128 * Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Integer
				+ ((128 * Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Numerator) /  Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Denominator)
				- 512;
		MS_P2 = 128 * Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Numerator
				- Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Denominator
				* ((128 * Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Numerator) / Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Denominator);
		MS_P3 = Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Denominator;

		tmp = (uint8_t) MS_P1;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P1_0_7 + 8 * MS_Channel, tmp);
		tmp = (uint8_t) (MS_P1 >> 8);
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P1_8_15 + 8 * MS_Channel, tmp);
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_P1_16_17);
		tmp &= ~MS_P1_16_17_MASK;
		tmp |= (uint8_t) (MS_P1_16_17_MASK & (MS_P1 >> 16));
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P1_16_17 + 8 * MS_Channel, tmp);

		tmp = (uint8_t) MS_P2;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P2_0_7 + 8 * MS_Channel, tmp);
		tmp = (uint8_t) (MS_P2 >> 8);
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P2_8_15 + 8 * MS_Channel, tmp);
		Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_P2_16_19 + 8 * MS_Channel);
		tmp &= ~MS_P2_16_19_MASK;
		tmp |= (uint8_t) (MS_P2_16_19_MASK & (MS_P2 >> 16));
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P2_16_19 + 8 * MS_Channel, tmp);

		tmp = (uint8_t) MS_P3;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P3_0_7 + 8 * MS_Channel, tmp);
		tmp = (uint8_t) (MS_P3 >> 8);
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P3_8_15 + 8 * MS_Channel, tmp);
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_P3_16_19 + 8 * MS_Channel);
		tmp &= ~MS_P3_16_19_MASK;
		tmp |= (uint8_t) (MS_P3_16_19_MASK & ((MS_P3 >> 16) << 4));
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_P3_16_19 + 8 * MS_Channel, tmp);

		//if next value is even integer and SS not enabled, enable integer mode
		if ((Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Numerator == 0) & ((Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Integer & 0x01) == 0) & (Si5351_ConfigStruct->SS.SS_Enable == OFF))
		{
			tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_INT + MS_Channel);
			tmp |= MS_INT_MASK;
			Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_INT + MS_Channel, tmp);

			//if next value in integer mode and if divider is equal to 4, enable DIVBY4
			if (Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Integer == 4)
			{
				tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_MS_DIVBY4 + 8 * MS_Channel);
				tmp |= MS_DIVBY4_MASK;
				Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS_DIVBY4 + 8 * MS_Channel, tmp);
			}
		}
	} else {
		//configure divider of Multisynth 6 and 7
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_MS67_P1 + (MS_Channel - MS6), (uint8_t)(Si5351_ConfigStruct->MS[MS_Channel].MS_Divider_Integer));
		//can be only even integers between 6 and 254, inclusive
	}
}

void Si5351_CLKPowerCmd(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_CLKChannelTypeDef CLK_Channel)
{
	uint8_t tmp, tmp_mask;

	//set CLK disable state
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_DIS_STATE + (CLK_Channel >> 2)); //increment the address by 1 if CLKx>=CLK4
	tmp_mask = CLK_DIS_STATE_MASK << ((CLK_Channel & 0x03)<<1); //shift the mask according to the selected channel
	tmp &= ~tmp_mask;
	tmp |= tmp_mask & ((Si5351_ConfigStruct->CLK[CLK_Channel].CLK_Disable_State) << ((CLK_Channel & 0x03)<<1));
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_DIS_STATE + (CLK_Channel >> 2), tmp);

	//set OEB pin
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_OEB);
	tmp_mask = 1 << CLK_Channel;
	tmp &= ~tmp_mask;
	if (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_Use_OEB_Pin == OFF)
	{
		tmp |= tmp_mask;
	}

	if (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_Enable == OFF) //disable clock
	{
		//power down the clock
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_EN);
		tmp |= 1 << CLK_Channel;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_EN, tmp);
	}

	if (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_PowerDown == ON) //power down clock
	{
		//power down output driver
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_PDN + CLK_Channel);
		tmp |= CLK_PDN_MASK;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_PDN + CLK_Channel, tmp);
	}

	if (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_PowerDown == OFF) //power up clock
	{
		//power up output driver
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_PDN + CLK_Channel);
		tmp &= ~CLK_PDN_MASK;
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_PDN + CLK_Channel, tmp);
	}

	if (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_Enable == ON) //enable clock
	{
		//power up the clock
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_EN);
		tmp &= ~(1 << CLK_Channel);
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_EN, tmp);
	}
}

void Si5351_CLKConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_CLKChannelTypeDef CLK_Channel)
{
	uint8_t tmp, tmp_mask;

	//set CLK source clock
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_SRC + CLK_Channel);
	tmp &= ~CLK_SRC_MASK;
	tmp |= CLK_SRC_MASK & Si5351_ConfigStruct->CLK[CLK_Channel].CLK_Clock_Source;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_SRC + CLK_Channel, tmp);

	//set CLK inversion
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_INV + CLK_Channel);
	tmp &= ~CLK_INV_MASK;
	if (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_Invert == ON)
	{
		tmp |= CLK_INV_MASK;
	}
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_INV + CLK_Channel, tmp);

	//set CLK current drive
	tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_IDRV + CLK_Channel);
	tmp &= ~CLK_IDRV_MASK;
	tmp |= CLK_IDRV_MASK & Si5351_ConfigStruct->CLK[CLK_Channel].CLK_I_Drv;
	Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_IDRV + CLK_Channel, tmp);

	if (CLK_Channel <= CLK5) //CLK6 and 7 are integer only, which causes several limitations
	{
		//set CLK phase offset
		tmp = CLK_PHOFF_MASK & (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_QuarterPeriod_Offset);
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_PHOFF + CLK_Channel, tmp);
		//set Rx divider
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_R_DIV + CLK_Channel * CLK_R_DIV_STEP);
		tmp &= ~CLK_R_DIV_MASK;
		tmp |= CLK_R_DIV_MASK & (Si5351_ConfigStruct->CLK[CLK_Channel].CLK_R_Div);
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_R_DIV + CLK_Channel * CLK_R_DIV_STEP, tmp);
	} else {
		//CLK6 and CLK7 have no fractional mode, so they lack the phase offset function

		//set Rx divider
		tmp_mask = CLK_R67_DIV_MASK << ((CLK_Channel-CLK6) << 2); //shift mask left by 4 if CLK7
		tmp = Si5351_ReadRegister(Si5351_ConfigStruct, REG_CLK_R67_DIV);
		tmp &= ~tmp_mask;
		tmp |= tmp_mask & ((Si5351_ConfigStruct->CLK[CLK_Channel].CLK_R_Div >> 4) << ((CLK_Channel-CLK6) << 2));
		Si5351_WriteRegister(Si5351_ConfigStruct, REG_CLK_R67_DIV, tmp);
	}
}

int Si5351_Init(Si5351_ConfigTypeDef *Si5351_ConfigStruct)
{
	uint32_t timeout = SI5351_TIMEOUT;
	uint8_t i;

	//wait for the 5351 to initialize
	while (Si5351_CheckStatusBit(Si5351_ConfigStruct, StatusBit_SysInit))
	{
		timeout--;
		if (timeout==0) return 1; //return 1 if initialization timed out
	}

	//configure oscillator, fanout, interrupts, VCXO
	Si5351_OSCConfig(Si5351_ConfigStruct);
	Si5351_InterruptConfig(Si5351_ConfigStruct);

	//configure PLLs
	for (i=PLL_A; i<=PLL_B; i++)
	{
		Si5351_PLLConfig(Si5351_ConfigStruct, i);
		Si5351_PLLReset(Si5351_ConfigStruct, i);
	}

	//configure Spread Spectrum
	Si5351_SSConfig(Si5351_ConfigStruct);

	//Configure Multisynths
	for (i=MS0; i<=MS7; i++)
	{
		Si5351_MSConfig(Si5351_ConfigStruct, i);
	}

	//configure outputs
	for (i=CLK0; i<=CLK7; i++)
	{
		Si5351_CLKConfig(Si5351_ConfigStruct, i);
	}

	//wait for PLLs to lock
	while (Si5351_CheckStatusBit(Si5351_ConfigStruct, StatusBit_SysInit | StatusBit_PLLA | StatusBit_PLLB))
	{
		timeout--;
		if (timeout==0) return 1; //return 1 if problem with any PLL
	}

	//clear sticky bits
	Si5351_ClearStickyBit(Si5351_ConfigStruct, StatusBit_SysInit | StatusBit_PLLA | StatusBit_PLLB);

	if (Si5351_ConfigStruct->f_CLKIN != 0) //if CLKIN used, check it as well
	{
		while (Si5351_CheckStatusBit(Si5351_ConfigStruct, StatusBit_CLKIN))
		{
			timeout--;
			if (timeout==0) return 1; //return 1 if initialization timed out
		}
		//clear CLKIN sticky bit
		Si5351_ClearStickyBit(Si5351_ConfigStruct, StatusBit_CLKIN);
	}

	if (Si5351_ConfigStruct->f_XTAL != 0) //if XTAL used, check it as well
	{
		while (Si5351_CheckStatusBit(Si5351_ConfigStruct, StatusBit_XTAL))
		{
			timeout--;
			if (timeout==0) return 1; //return 1 if initialization timed out
		}
		//clear XTAL sticky bit
		Si5351_ClearStickyBit(Si5351_ConfigStruct, StatusBit_XTAL);
	}

	//power on or off the outputs
	for (i=CLK0; i<=CLK7; i++)
	{
		Si5351_CLKPowerCmd(Si5351_ConfigStruct, i);
	}

	return 0;
}
