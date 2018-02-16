/*
 * ssd1306.c
 *
 *  Created on: Jan 14, 2018
 *      Author: msboss
 */

#include <stdlib.h>
#include "stm32f0xx_i2c.h"
#include "myFont.h"
#include "ssd1306.h"
#include "board.h"

void SSD1306_StructInit(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, SSD1306_DisplayModelTypeDef SSD1306_DisplayModel)
{
	SSD1306_ConfigStruct->Address_Column_End=127;
	SSD1306_ConfigStruct->Address_Column_Start=0;
	SSD1306_ConfigStruct->Address_Mode=AddressMode_Page;
	SSD1306_ConfigStruct->Address_Page_End=1;
	SSD1306_ConfigStruct->Address_Page_Start=0;

	SSD1306_ConfigStruct->Display_Contrast=255;
	SSD1306_ConfigStruct->Display_Flip_H=OFF;
	SSD1306_ConfigStruct->Display_Flip_V=OFF;
	SSD1306_ConfigStruct->Display_Model=DisplayModel_UG2864KSWEG01;
	SSD1306_ConfigStruct->Display_Polarity=DisplayPolarity_Normal;
	SSD1306_ConfigStruct->Display_Source=DisplaySource_RAM;
	SSD1306_ConfigStruct->Display_Start_Line=0;
	SSD1306_ConfigStruct->Display_State=ON;
	SSD1306_ConfigStruct->Display_Vertical_Offset=0;

	SSD1306_ConfigStruct->GR_DrawMethod=DrawMethod_Straight;

	SSD1306_ConfigStruct->HW_COMSeqRemap_Default = COMSeq_Alternate_Nonremapped;
	SSD1306_ConfigStruct->HW_CPump_State=ON;
	SSD1306_ConfigStruct->HW_Clock_Divider=0;
	SSD1306_ConfigStruct->HW_Clock_Frequency=8; //can be faster, but then the capacitors start making unpleasant sounds
	SSD1306_ConfigStruct->HW_I2C_Address=SSD1306_ADDRESS_0;
	SSD1306_ConfigStruct->HW_Precharge_Phase_1=1;
	SSD1306_ConfigStruct->HW_Precharge_Phase_2=15;
	SSD1306_ConfigStruct->HW_VCOMHLevel=VCOMHLevel_Fucked; //undocumented VCOMH level, measured around 0.99xVcc
	SSD1306_ConfigStruct->HW_Vertical_Resolution=64;

	SSD1306_ConfigStruct->I2Cx=I2C1;
	//SSD1306_ConfigStruct->SSD1306_Framebuffer //you need to supply this one manually

	SSD1306_ConfigStruct->Scroll_Fixed_Rows=0;
	SSD1306_ConfigStruct->Scroll_Mode=ScrollMode_Horizontal_Left;
	SSD1306_ConfigStruct->Scroll_Scrolling_Rows=0;
	SSD1306_ConfigStruct->Scroll_State=OFF;

	if(SSD1306_DisplayModel==DisplayModel_UG2864KSWEG01)
	{
		SSD1306_ConfigStruct->Display_Contrast=255; //for 3V3 supply and 390K resistor
		SSD1306_ConfigStruct->Display_Model=DisplayModel_UG2864KSWEG01;
		SSD1306_ConfigStruct->HW_COMSeqRemap_Default = COMSeq_Alternate_Nonremapped;
		SSD1306_ConfigStruct->HW_Clock_Divider=0;
		SSD1306_ConfigStruct->HW_Clock_Frequency=8; //can be faster, but then the capacitors start making unpleasant sounds
		SSD1306_ConfigStruct->HW_I2C_Address=SSD1306_ADDRESS_0;
		SSD1306_ConfigStruct->HW_Precharge_Phase_1=1;
		SSD1306_ConfigStruct->HW_Precharge_Phase_2=15;
		SSD1306_ConfigStruct->HW_VCOMHLevel=VCOMHLevel_Fucked; //undocumented VCOMH level, measured around 0.99xVcc
		SSD1306_ConfigStruct->HW_Vertical_Resolution=64;
		SSD1306_ConfigStruct->I2Cx=I2C1;
	}
}

int SSD1306_Init(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct)
{
    uint8_t error=0;
	uint8_t command[8];

    command[0]=COMMAND_DISPLAY_OFF;				//set display off
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

	command[0]=COMMAND_HW_SET_CLOCK;			// Set Display Clk
	command[1]=(SSD1306_ConfigStruct->HW_Clock_Divider & 0x0F)|(SSD1306_ConfigStruct->HW_Clock_Frequency<<4);
	error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SET_MULTIPLEX;		//set display multiplex ratio
    command[1]=SSD1306_ConfigStruct->HW_Vertical_Resolution-1;
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SET_DISPLAY_OFFSET;	//set display offset
    command[1]=SSD1306_ConfigStruct->Display_Vertical_Offset;
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SET_START_LINE | (0x3F & SSD1306_ConfigStruct->Display_Start_Line);	//set start line to 0
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=COMMAND_CP_SET;					//setup charge pump
    if (SSD1306_ConfigStruct->HW_CPump_State==ON)
    {
    	command[1]=CP_ENABLE;
    }else{
    	command[1]=CP_DISABLE;
    }
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SEG_REMAP0_ON;		//map SEG0 to COL 127
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=COMMAND_HW_SCAN_DIR_REVERSE;		//set reverse scan direction
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=COMMAND_HW_SET_COM;				//set COM pins mapping
    command[1]=SSD1306_ConfigStruct->HW_COMSeqRemap_Default;
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SET_CONTRAST;			//set contrast
    command[1]=SSD1306_ConfigStruct->Display_Contrast;
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SET_PRECHARGE;		//set precharge times
    command[1]=(SSD1306_ConfigStruct->HW_Precharge_Phase_1 & 0x0F)|(SSD1306_ConfigStruct->HW_Precharge_Phase_2 <<4);
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=COMMAND_HW_SET_VCOMH_DESEL_LVL;	//set VCOM regulator level
    command[1]=SSD1306_ConfigStruct->HW_VCOMHLevel;		//set to strange, but recommended value
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    command[0]=SSD1306_ConfigStruct->Display_Source;	//display RAM contents
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=SSD1306_ConfigStruct->Display_Polarity;	//display noninverted image
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=COMMAND_ADDR_SET_COLUMN_LOWER  | (0x0F & SSD1306_ConfigStruct->Address_Column_Start);	//set column start address
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);
    command[0]=COMMAND_ADDR_SET_COLUMN_HIGHER | (0x0F & (SSD1306_ConfigStruct->Address_Column_Start >>4 ));
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=COMMAND_HW_SET_START_LINE | (0x3F & SSD1306_ConfigStruct->Display_Start_Line);	//set start line to 0
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    command[0]=COMMAND_ADDR_SET_MODE;	//set addressing mode
    command[1]=SSD1306_ConfigStruct->Address_Mode;		//to page addressing
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 2, &command);

    if (SSD1306_ConfigStruct->Display_State == ON)
    {
    	command[0]=COMMAND_DISPLAY_ON;
    }else{
    	command[0]=COMMAND_DISPLAY_OFF;
    }	//turn display ON or OFF
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

    return error;
}

int SSD1306_SendCommand(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct,  uint8_t command_length, uint8_t *command_data)
{
	uint32_t error_wait;

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_BUSY) == SET)
	{
		error_wait--;
		if (error_wait==0)
		{
			I2C_SoftwareResetCmd(SSD1306_ConfigStruct->I2Cx);
			return 1;
		}
	}
	//wait for I2C to get ready, if not ready in time, reset I2C and return

	I2C_TransferHandling(SSD1306_ConfigStruct->I2Cx, SSD1306_ConfigStruct->HW_I2C_Address, command_length+1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	//send address+RW bit

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_TXIS) == RESET)
	{
		error_wait--;
		if (error_wait==0)
		{
			return 1;
		}
	}
	//wait for address to be sent

	I2C_SendData(SSD1306_ConfigStruct->I2Cx, (uint8_t)COMMAND_BYTE);
	//send command prefix

	uint8_t i=0;
	for(i=0; i<command_length; i++)
	{
		error_wait = I2C_TIMEOUT;
		while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_TXIS) == RESET)
		{
			error_wait--;
			if (error_wait==0) return 1;
		}
		//wait for data to be sent

		I2C_SendData(SSD1306_ConfigStruct->I2Cx, (uint8_t)command_data[i]);
		//send command
	}

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_STOPF) == RESET)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for STOP flag

	I2C_ClearFlag(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_STOPF);	//clear STOP flag
	return 0;
}

int SSD1306_DrawBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct)
{
	uint32_t error_wait;
	uint8_t x=0, y=0, error=0;
    uint8_t command[8];

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_BUSY) == SET)
	{
		error_wait--;
		if (error_wait==0)
		{
			I2C_SoftwareResetCmd(SSD1306_ConfigStruct->I2Cx);
			return 1;
		}
	}

	command[0]=COMMAND_ADDRESS_SET_COLUMN;	//set start and end columns of addressing
	command[1]=0;
	command[2]=127;
    error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 3, &command);


    for(y=0; y<8; y++)
    {
        command[0]=COMMAND_ADDR_SET_PAGE_START | (0x07 & y);	//set start and end pages of addressing
        error |= SSD1306_SendCommand(SSD1306_ConfigStruct, 1, &command);

        I2C_TransferHandling(SSD1306_ConfigStruct->I2Cx, SSD1306_ConfigStruct->HW_I2C_Address, 129, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
        //send address and prepare to send 129 bytes

    	error_wait = I2C_TIMEOUT;
    	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_TXIS) == RESET)
    	{
    		error_wait--;
    		if (error_wait==0)
    		{
    			return 1;
    		}
    	}
    	//wait for the address to be sent

        I2C_SendData(SSD1306_ConfigStruct->I2Cx, DATA_BYTE);
        //send data prefix

        for(x=0; x<128; x++) //send the 128 bytes of buffer
        {
        	error_wait = I2C_TIMEOUT;
        	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_TXIS) == RESET)
        	{
        		error_wait--;
        		if (error_wait==0)
        		{
        			return 1;
        		}
        	}
        	//wait for the data prefix to be sent (first loop) / wait for data to be sent
            I2C_SendData(SSD1306_ConfigStruct->I2Cx, (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[(128*y)+x]);
            //send byte of the buffer
        }
    }

	error_wait = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_STOPF) == RESET)
	{
		error_wait--;
		if (error_wait==0) return 1;
	}
	//wait for STOP flag

	I2C_ClearFlag(SSD1306_ConfigStruct->I2Cx, I2C_FLAG_STOPF);	//clear STOP flag
    return error;
}

void SSD1306_ClearDisplayBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct){
// Clear all bytes of the 128x64 display buffer given by buffer_pointer
    uint16_t x;

    for(x=0; x<FRAMEBUFFER_SIZE; x++)
    {
        	(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[x] = 0x00;
    }
}

void SSD1306_DrawPixelToBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, uint8_t x, uint8_t y){
// Turns on the pixel at (x,y) in the local buffer
    #define SSD1306_HORIZONTAL_RES  128

    uint8_t which_bit, which_page, bit_mask = 0;
    uint16_t which_byte;

    if ((x>127)|(y>(SSD1306_ConfigStruct->HW_Vertical_Resolution))) return;

    which_page = y / 8;             // There are 8 vertical pixels per page
    which_byte = x + (SSD1306_HORIZONTAL_RES * which_page);
    which_bit = y % 8;

    bit_mask = (1<<which_bit);

    switch(SSD1306_ConfigStruct->GR_DrawMethod)
    {
    	case DrawMethod_OFF:
        	(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] & ~bit_mask;
        	break;
    	case DrawMethod_XOR:
    		(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] ^ bit_mask;
    		break;
    	default:
    	case DrawMethod_ON:
    		(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] | bit_mask;
    		break;
    }
}

void SSD1306_DrawCharToBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, uint8_t x, uint8_t page_num, char which_char){
// Draws 'which_char' to the display buffer given by buffer_pointer at coord (x, page_num)
    uint8_t i;
    uint16_t which_byte = 0;

    for(i=0; i<5; i++)
    {
        which_byte = x + ((page_num)*128) + i;
        if (which_byte >= FRAMEBUFFER_SIZE) break;
		switch(SSD1306_ConfigStruct->GR_DrawMethod)
		{
			case DrawMethod_ON:
				(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] |  Ascii_8x5_font[which_char-ASCII_8x5_OFFSET][i];
				break;
			case DrawMethod_OFF:
				(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] & ~Ascii_8x5_font[which_char-ASCII_8x5_OFFSET][i];
				break;
			case DrawMethod_XOR:
				(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = (*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] ^  Ascii_8x5_font[which_char-ASCII_8x5_OFFSET][i];
				break;
			case DrawMethod_Straight:
			default:
				(*(SSD1306_ConfigStruct->SSD1306_Framebuffer))[which_byte] = Ascii_8x5_font[which_char-ASCII_8x5_OFFSET][i];
				break;
		}
    }
}

void SSD1306_DrawStringToBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, uint8_t x, uint8_t page_num, char string[]){
	// Draws 'string' to the display buffer (buffer pointer) at coord (x, page_num)
	    uint8_t i=0;
	    while(string[i] != 0){
	        SSD1306_DrawCharToBuffer(SSD1306_ConfigStruct, x+(6*i), page_num, string[i]);
	        i++;
	        if (i==255) break;
	    }
	}

void SSD1306_DrawLine(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, int8_t x_start, int8_t y_start, int8_t x_end, int8_t y_end)
{
	if ((x_start==x_end)&(y_start==y_end)) return;
	if ( abs(x_end-x_start) >= abs(y_end-y_start) )
		{
			//if line more horizontal than vertical
			uint16_t x;
			if (x_end<x_start)
			{
				int8_t temp;
				temp=x_end;
				x_end=x_start;
				x_start=temp;
				temp=y_end;
				y_end=y_start;
				y_start=temp;
			}
			for (x=x_start; x<=x_end; x++) SSD1306_DrawPixelToBuffer(SSD1306_ConfigStruct, x, y_start+((y_end-y_start)*((int16_t)x-x_start))/(x_end-x_start));
		} else {
			//if line more vertical than horizontal
			uint16_t y;
			if (y_end<y_start)
			{
				int8_t temp;
				temp=x_end;
				x_end=x_start;
				x_start=temp;
				temp=y_end;
				y_end=y_start;
				y_start=temp;
			}
			for (y=y_start; y<=y_end; y++) SSD1306_DrawPixelToBuffer(SSD1306_ConfigStruct, x_start+((x_end-x_start)*((int16_t)y-y_start))/(y_end-y_start), y);
		}
}

void SSD1306_DrawRectangle(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, int8_t x_topleft, int8_t y_topleft, int8_t x_bottomright, int8_t y_bottomright)
{
	SSD1306_DrawLine(SSD1306_ConfigStruct, x_topleft, 		y_topleft, 		x_bottomright, 	y_topleft);
	SSD1306_DrawLine(SSD1306_ConfigStruct, x_topleft, 		y_bottomright, 	x_bottomright, 	y_bottomright);
	SSD1306_DrawLine(SSD1306_ConfigStruct, x_topleft, 		y_topleft, 		x_topleft, 		y_bottomright);
	SSD1306_DrawLine(SSD1306_ConfigStruct, x_bottomright, 	y_topleft, 		x_bottomright, 	y_bottomright);
}
