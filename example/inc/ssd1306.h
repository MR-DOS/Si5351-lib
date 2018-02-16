/*
 * ssd1306.h
 *
 *  Created on: Jan 14, 2018
 *      Author: msboss
 */

#ifndef SSD1306_H_
#define SSD1306_H_

#include "board.h"

#define	SSD1306_COLUMNS		128
#define SSD1306_ROWS		64
#define FRAMEBUFFER_SIZE	(SSD1306_COLUMNS*SSD1306_ROWS/8)

#ifdef I2C_TIMEOUT
	#undef I2C_TIMEOUT
#endif
#define I2C_TIMEOUT 	100000		//I2C timeout for wait loops

#define SSD1306_ADDRESS_0				0x78	// D/~C pin drives address bit SA0
#define	SSD1306_ADDRESS_1				0x7A

#define CONTROL_BYTE_CONTINUATION_MASK	0x80	//mask for continuation bit, normally not used
#define CONTROL_BYTE_DATA_COMMAND_MASK	0x40	//mask for D/~C, 0 for command, 1 for data

#define CONTROL_BYTE_COMMAND			0x00	//command starting control byte
#define CONTROL_BYTE_DATA				CONTROL_BYTE_DATA_COMMAND_MASK	//data starting control byte

#define COMMAND_BYTE 					CONTROL_BYTE_COMMAND //duplicities with simpler names
#define DATA_BYTE						CONTROL_BYTE_DATA

//SSD1306 specific commands
#define COMMAND_ADDR_SET_COLUMN_LOWER 	0x00	//set lower  nibble of column start address for page addr mode, should be ORed with the nibble [3:0]
#define COMMAND_ADDR_SET_COLUMN_HIGHER	0x10	//set higher nibble of column start address for page addr mode, should be ORed with the nibble [3:0]
#define COMMAND_ADDR_SET_MODE			0x20	//set memory addressing mode, mode should follow in next byte
	#define	ADDR_MODE_HORIZONTAL			0x00	//horizontal addressing mode
	#define	ADDR_MODE_VERTICAL				0x01	//vertical addressing mode
	#define ADDR_MODE_PAGE					0x02	//page addressing mode
	//mode 0x03 is invalid
#define COMMAND_ADDRESS_SET_COLUMN		0x21	//set column start and end address for H/V addr mode
	//first byte sets start column address
	//second byte sets end column address
#define COMMAND_ADDRESS_SET_PAGE		0x22	//set page start and end address for H/V addr mode
	//first byte sets start page address
	//second byte sets end page address

#define COMMAND_SCROLL_SET_H_RIGHT		0x26	//setup horizontal scrolling, see next command for info on parameters
	//first byte should be 0x00, dummy byte
	//second byte defines start page address [2..0]
	//third byte sets interval between scrolling steps [2..0]
		// 5,64,128,256,3,4,25,2
	//fourth byte sets end page address [2..0], must be larger or equal to second byte
	//fifth byte should be 0x00, dummy byte
	//sixth byte should be 0xFF, dummy byte
#define COMMAND_SCROLL_SET_H_LEFT		0x27	//setup horizontal scrolling
	//parameters the same as in 0x26
#define COMMAND_SCROLL_SET_VH_RIGHT		0x29	//setup vertical and right horizontal scrolling
	//parameters the same as in 0x26, except fifth byte [5..0] sets vertical scrolling offset and sixth byte is omitted
#define COMMAND_SCROLL_SET_VH_LEFT		0x2A	//setup vertical and left horizontal scrolling
	//parameters the same as in 0x26, except fifth byte [5..0] sets vertical scrolling offset and sixth byte is omitted
#define COMMAND_SCROLL_DEACTIVATE		0x2E	//deactivate scrolling, RAM needs to be rewritten after this command
#define COMMAND_SCROLL_ACTIVATE			0x2F	//activate scrolling

#define COMMAND_HW_SET_START_LINE		0x40	//set display start line, should be ORed with the line [5:0]

#define COMMAND_HW_SET_CONTRAST			0x81	//set contrast
	//second byte sets contrast (0..255)

#define COMMAND_CP_SET					0x8D	//setup charge pump
	#define CP_DISABLE						0x10	//disable charge pump
	#define CP_ENABLE						0x14	//enable CP during display on

#define COMMAND_HW_SEG_REMAP0_OFF		0xA0	//map SEG0 to column 0 - flipped H display
#define COMMAND_HW_SEG_REMAP0_ON		0xA1	//map SEG0 to column 127 - nonflipped  H display

#define COMMAND_SCROLL_SET_V_AREA		0xA3	//set vertical scroll area
	//first byte [5:0] sets number of fixed rows
	//second byte [6:0] sets number of scrolling rows
		//the sum of the bytes (as well as the second byte alone) must be <= MUX ratio
		//vertical scrolling offset must be < second byte
		//display start line must be < second byte
		//last row of the scroll area shifts to the first row of scroll area

#define COMMAND_DISPLAY_RAM				0xA4	//display RAM contents
#define COMMAND_DISPLAY_WHITE			0xA5	//display whole white display
#define COMMAND_DISPLAY_NORMAL			0xA6	//display noninverted image
#define	COMMAND_DISPLAY_INVERTED		0xA7	//display inverted image

#define COMMAND_HW_SET_MULTIPLEX		0xA8	//set mux ratio to N+1
	//first byte sets N (15..63)

#define	COMMAND_DISPLAY_OFF				0xAE	//turn OFF display
#define	COMMAND_DISPLAY_ON				0xAF	//turn IN display

#define COMMAND_ADDR_SET_PAGE_START		0xB0	//set start page address for page addr mode, should be ORed with PAGE [2..0]

#define COMMAND_HW_SCAN_DIR_NORMAL		0xC0	//scan from COM0 to COM[N-1]
#define COMMAND_HW_SCAN_DIR_REVERSE		0xC8	//scan from COM[N-1] to COM[0]

#define COMMAND_HW_SET_DISPLAY_OFFSET	0xD3	//set display vertical offset
	//first byte sets offset (0..63)
#define COMMAND_HW_SET_CLOCK			0xD5	//set clock frequency and divider
	//first byte sets clock, [3:0]+1=divide ratio, [7:4] sets frequency (higher value=higher freq)
#define COMMAND_HW_SET_PRECHARGE		0xD9	//set pre-charge period
	//first byte sets precharge period, [3:0]=phase 1 precharge cycles, [7:4]=phase 2 precharge cycles
#define COMMAND_HW_SET_COM				0xDA	//configure COM Pins
	//first byte sets COM configuration
	#define HW_COM_SEQ_DISBL				0x02	//sequential COM pin configuration, disable COM L/R remap, default for 128x32
	#define HW_COM_ALT_DISBL				0x12	//alternate  COM pin configuration, disable COM L/R remap, default for 128x64
	#define HW_COM_SEQ_ENABL				0x22	//sequential COM pin configuration, enable  COM L/R remap
	#define HW_COM_ALT_ENABL				0x32	//alternate  COM pin configuration, enable  COM L/R remap
#define COMMAND_HW_SET_VCOMH_DESEL_LVL	0xDB	//set VCOMH deselect level
	#define HW_VCOMH_DESEL_LVL_065			0x00	//set deselect level to 0.65xVcc
	#define HW_VCOMH_DESEL_LVL_077			0x20	//set deselect level to 0.77xVcc
	#define HW_VCOMH_DESEL_LVL_083			0x30	//set deselect level to 0.83xVcc
	#define HW_VCOMH_DESEL_LVL_FUCKED		0x40	//this value appears to be valid and appears in official initiation protocol
													//however, according to datasheet, it is mentioned as valid setting
													//datasheet suggests maximum VCOM=0.9*Vcc, so this may or may NOT be that value
													//It is said to have maximum brightness. Fuck this Chinese shit.

#define COMMAND_NOP						0xE3	//NOP command

//UG-2864KSWEG01 specific commands - probably just normal SSD1306 functions not mentioned in datasheet

typedef uint8_t Framebuffer[FRAMEBUFFER_SIZE];
extern Framebuffer display_buffer;

typedef enum {BITCOLOR_BLACK=0, BITCOLOR_WHITE=1} BitColor;

#ifndef ENABLESTATE
#define ENABLESTATE
typedef enum
{
	OFF = 0,
	ON = 1
} EnableState;
#endif

typedef enum
{
	AddressMode_Horizontal = ADDR_MODE_HORIZONTAL,
	AddressMode_Vertical   = ADDR_MODE_VERTICAL,
	AddressMode_Page	   = ADDR_MODE_PAGE
} SSD1306_AddressModeTypeDef;

typedef enum
{
	ScrollMode_Horizontal_Left = COMMAND_SCROLL_SET_H_LEFT,
	ScrollMode_Horizontal_Right = COMMAND_SCROLL_SET_H_RIGHT,
	ScrollMode_Horizontal_Vertical_Left = COMMAND_SCROLL_SET_VH_LEFT,
	ScrollMode_Horizontal_Vertical_Right = COMMAND_SCROLL_SET_VH_RIGHT
} SSD1306_ScrollModeTypeDef;

typedef enum
{
	DisplaySource_RAM = COMMAND_DISPLAY_RAM,
	DisplaySource_White = COMMAND_DISPLAY_WHITE
} SSD1306_DisplaySourceTypeDef;

typedef enum
{
	DisplayPolarity_Normal = COMMAND_DISPLAY_NORMAL,
	DisplayPolarity_Inverted = COMMAND_DISPLAY_INVERTED
} SSD1306_DisplayPolarityTypeDef;

typedef enum
{
	COMSeq_Sequential_Nonremapped = HW_COM_SEQ_DISBL,
	COMSeq_Sequential_Remapped = HW_COM_SEQ_ENABL,
	COMSeq_Alternate_Nonremapped= HW_COM_ALT_DISBL,
	COMSeq_Alternate_Remapped = HW_COM_ALT_ENABL
} SSD1306_COMSeqRemapTypeDef;

typedef enum
{
	VCOMHLevel_065,
	VCOMHLevel_077,
	VCOMHLevel_083,
	VCOMHLevel_Fucked
} SSD1306_VCOMHLevelTypeDef;

typedef enum
{
	DisplayModel_UG2864KSWEG01
} SSD1306_DisplayModelTypeDef;

typedef enum
{
	COMRemap_NonRemapped = COMMAND_HW_SEG_REMAP0_OFF,
	COMRemap_Remapped = COMMAND_HW_SEG_REMAP0_ON
} SSD1306_COMRemapTypeDef;

typedef enum
{
	DrawMethod_ON,		//turn pixels on
	DrawMethod_OFF,		//turn pixels off
	DrawMethod_XOR,		//XOR with existing pixels
	DrawMethod_Straight	//use for text to make it readable - simply writes exact pixels
} SSD1306_DrawMethodTypeDef;

typedef struct
{
	SSD1306_DisplayModelTypeDef Display_Model;

	SSD1306_AddressModeTypeDef Address_Mode;
	uint8_t Address_Column_Start;
	uint8_t Address_Column_End;
	uint8_t Address_Page_Start;
	uint8_t Address_Page_End;

	SSD1306_ScrollModeTypeDef Scroll_Mode;
	EnableState Scroll_State;
	uint8_t Scroll_Fixed_Rows;
	uint8_t Scroll_Scrolling_Rows;

	uint8_t Display_Start_Line;
	uint8_t Display_Contrast;
	EnableState Display_Flip_H;
	EnableState Display_Flip_V;
	SSD1306_DisplaySourceTypeDef Display_Source;
	SSD1306_DisplayPolarityTypeDef Display_Polarity;
	uint8_t Display_Vertical_Offset;
	EnableState Display_State;

	EnableState HW_CPump_State;
	uint8_t HW_Vertical_Resolution;
	uint8_t HW_Clock_Frequency;
	uint8_t HW_Clock_Divider;
	uint8_t HW_Precharge_Phase_1;
	uint8_t HW_Precharge_Phase_2;
	SSD1306_VCOMHLevelTypeDef HW_VCOMHLevel;
	SSD1306_COMSeqRemapTypeDef HW_COMSeqRemap_Default;
	SSD1306_COMRemapTypeDef HW_COMRemap_Default;

	SSD1306_DrawMethodTypeDef GR_DrawMethod;

	I2C_TypeDef *I2Cx;
	uint8_t HW_I2C_Address;
	Framebuffer *SSD1306_Framebuffer;
} SSD1306_ConfigTypeDef;

void SSD1306_StructInit(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, SSD1306_DisplayModelTypeDef SSD1306_DisplayModel);
int SSD1306_Init(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct);
int SSD1306_SendCommand(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct,  uint8_t command_length, uint8_t *command_data);
int SSD1306_DrawBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct);
void SSD1306_ClearDisplayBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct);
void SSD1306_DrawPixelToBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, uint8_t x, uint8_t y);
void SSD1306_DrawCharToBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, uint8_t x, uint8_t page_num, char which_char);
void SSD1306_DrawStringToBuffer(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, uint8_t x, uint8_t page_num, char string[]);
void SSD1306_DrawLine(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, int8_t x_start, int8_t y_start, int8_t x_end, int8_t y_end);
void SSD1306_DrawRectangle(SSD1306_ConfigTypeDef *SSD1306_ConfigStruct, int8_t x_topleft, int8_t y_topleft, int8_t x_bottomleft, int8_t y_bottomleft);
#endif /* SSD1306_H_ */
