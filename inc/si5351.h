/*
 * si5351.h
 *
 *  Created on: Feb 16, 2018
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

#ifndef SI5351_H_
#define SI5351_H_

#define SI5351_I2C_ADDRESS		0xC0		//default I2C address of Si5351
#define SI5351_I2C_PERIPHERAL	I2C1		//default I2C interface

#define SI5351_XTAL_FREQ		25000000	// sets default value, 25000000 for 25 MHz, 27000000 for 27 MHz
#define SI5351_CLKIN_FREQ		0			// set in Hz

#ifdef I2C_TIMEOUT
	#undef I2C_TIMEOUT
#endif
#define I2C_TIMEOUT 			100000		//I2C timeout for wait loops

#define SI5351_TIMEOUT			(I2C_TIMEOUT * 10)

#ifndef ENABLESTATE
	#define ENABLESTATE
	typedef enum
	{
		OFF = 0,
		ON = 1
	} EnableState;
#endif

/*
 * This section contains register addresses and bit masks for
 * the device status registers.
 */

#define REG_DEV_STATUS			0
	#define DEV_SYS_INIT_MASK		0x80
	#define DEV_LOL_B_MASK			0x40
	#define DEV_LOL_A_MASK			0x20
	#define DEV_LOS_CLKIN_MASK		0x10
	#define DEV_LOS_XTAL_MASK		0x08
	#define DEV_REVID_MASK			0x03

#define REG_DEV_STICKY			1
	#define DEV_STKY_SYS_INIT_MASK	0x80
	#define DEV_STKY_LOL_B_MASK		0x40
	#define DEV_STKY_LOL_A_MASK		0x20
	#define DEV_STKY_LOS_CLKIN_MASK	0x10
	#define DEV_STKY_LOS_XTAL_MASK	0x08

#define REG_INT_MASK			2
	#define INT_MASK_SYS_INIT_MASK	0x80
	#define INT_MASK_LOL_B_MASK		0x40
	#define INT_MASK_LOL_A_MASK		0x20
	#define INT_MASK_LOS_CLKIN_MASK	0x10
	#define INT_MASK_LOS_XTAL_MASK	0x08

/*
 * This section contains data structures for configuring the
 * oscillator, VCXO and CLKIN section.
 */

#define REG_XTAL_CL		183
#define XTAL_CL_MASK	0xC0
#define PLL_CL_MASK		0x36

//this sets the crystal load capacitance
typedef enum
{
	XTAL_Load_4_pF  = 0x00,
	XTAL_Load_6_pF  = 0x40,
	XTAL_Load_8_pF  = 0x80,
	XTAL_Load_10_pF = 0xC0
} Si5351_XTALLoadTypeDef;

//The following is an unexplained parameter. However someone from SiLabs called it "VCO load cap".
//Lower settings seem to be more stable on higher frequencies, higher settings are more stable on lower frequencies allowing to tune the PLL to <200 MHz.
typedef enum
{
	PLL_Capacitive_Load_0 = 0,
	PLL_Capacitive_Load_1 = 1,
	PLL_Capacitive_Load_2 = 2
} Si5351_PLLCapacitiveLoadTypeDef;

#define REG_CLKIN_DIV	15
#define CLKIN_MASK		0xC0

//this sets the CLKIN pre-divider, after division, CLKIN should
//fall between 10-40 MHz
typedef enum
{
	CLKINDiv_Div1 = 0x00,
	CLKINDiv_Div2 = 0x40,
	CLKINDiv_Div4 = 0x80,
	CLKINDiv_Div8 = 0xC0
} Si5351_CLKINDivTypeDef;

#define REG_FANOUT_EN		 	187
#define FANOUT_CLKIN_EN_MASK	0x80
#define FANOUT_XO_EN_MASK		0x40
#define FANOUT_MS_EN_MASK		0x10

#define REG_VCXO_PARAM_0_7	 	162
#define REG_VCXO_PARAM_8_15	 	163
#define REG_VCXO_PARAM_16_21 	164
#define VCXO_PARAM_16_21_MASK	0x3F
#define VCXO_PARAM_MASK			0x003FFFFF

#define APR_MINIMUM				30		//minimum pull range
#define APR_MAXIMUM				240		//maximum pull range

#define CLKIN_MINIMUM			10000	//minimum CLKIN frequency after division in kHz
#define CLKIN_MAXIMUM			40000	//maximum CLKIN frequency after division in kHz

typedef struct
{
	Si5351_XTALLoadTypeDef OSC_XTAL_Load; //capacitive load of XTAL, 10pF by default
	Si5351_CLKINDivTypeDef CLKIN_Div;	  //CLKIN predivision, input f to PLL must be 10-40 MHz
	uint8_t VCXO_Pull_Range_ppm; 		  //can range from +-30 ppm to 240ppm
} Si5351_OSCConfigTypeDef;

/*
 * This section contains data structures for configuring the
 * PLL (PLLA and PLLB)
 */

#define REG_PLL_CLOCK_SOURCE	15
#define PLLA_CLOCK_SOURCE_MASK	0x04
#define PLLB_CLOCK_SOURCE_MASK	0x08

//this selects the clock source for the PLL
typedef enum
{
	PLL_Clock_Source_XTAL  = 0x00,
	PLL_Clock_Source_CLKIN = 0x0C	//0x04 for PLLA, 0x08 for PLLB, use mask!
} Si5351_PLLClockSourceTypeDef;

#define REG_FB_INT				22
#define FB_INT_MASK				0x40

#define REG_PLL_RESET			177
#define PLLA_RESET_MASK			0x20
#define PLLB_RESET_MASK			0x80

#define REG_MSN_P1_0_7			30
#define REG_MSN_P1_8_15			29
#define REG_MSN_P1_16_17		28
#define MSN_P1_16_17_MASK		0x03

#define REG_MSN_P2_0_7			33
#define REG_MSN_P2_8_15			32
#define REG_MSN_P2_16_19		31
#define MSN_P2_16_19_MASK		0x0F

#define REG_MSN_P3_0_7			27
#define REG_MSN_P3_8_15			26
#define REG_MSN_P3_16_19		31
#define MSN_P3_16_19_MASK			0xF0

#define MSNA_MSNB_OFFSET		8

typedef struct
{
	uint32_t PLL_Multiplier_Integer;
	uint32_t PLL_Multiplier_Numerator;
	uint32_t PLL_Multiplier_Denominator;
	Si5351_PLLClockSourceTypeDef PLL_Clock_Source;
	Si5351_PLLCapacitiveLoadTypeDef PLL_Capacitive_Load;
} Si5351_PLLConfigTypeDef;

/*
 * This section contains data structures for configuring the
 * Spread Spectrum feature.
 */

#define REG_SSC_MODE		151
#define SSC_MODE_MASK		0x80

//this selects the Spread Spectrum mode
typedef enum
{
	SS_Mode_DownSpread 	= 0x00,
	SS_Mode_CenterSpread = 0x80
} Si5351_SSModeTypeDef;

typedef enum
{
	SS_NCLK_0 = 0x00,
	SS_NCLK_1 = 0x10,
	SS_NCLK_2 = 0x20,
	SS_NCLK_3 = 0x30,
	SS_NCLK_4 = 0x40,
	SS_NCLK_5 = 0x50,
	SS_NCLK_6 = 0x60,
	SS_NCLK_7 = 0x70,
	SS_NCLK_8 = 0x80,
	SS_NCLK_9 = 0x90,
	SS_NCLK_10 = 0xA0,
	SS_NCLK_11 = 0xB0,
	SS_NCLK_12 = 0xC0,
	SS_NCLK_13 = 0xD0,
	SS_NCLK_14 = 0xE0,
	SS_NCLK_15 = 0xF0
} Si5351_SSNCLKTypeDef;

#define REG_SSDN_P1_0_7		153
#define REG_SSDN_P1_8_11	154
#define SSDN_P1_8_11_MASK	0x0F

#define REG_SSDN_P2_0_7		150
#define REG_SSDN_P2_8_14	149
#define SSDN_P2_8_14_MASK	0x7F

#define REG_SSDN_P3_0_7		152
#define REG_SSDN_P3_8_14	151
#define SSDN_P3_8_14_MASK	0x7F

#define REG_SSUDP_0_7		155
#define REG_SSUDP_8_11		154
#define SSUDP_8_11_MASK		0xF0

#define REG_SSUP_P1_0_7		160
#define REG_SSUP_P1_8_11	161
#define SSUP_P1_8_11_MASK	0x0F

#define REG_SSUP_P2_0_7		157
#define REG_SSUP_P2_8_14	156
#define SSUP_P2_8_14_MASK	0x7F

#define REG_SSUP_P3_0_7		159
#define REG_SSUP_P3_8_14	158
#define SSUP_P3_8_14_MASK	0x7F

#define REG_SSC_EN			149
#define SSC_EN_MASK			0x80

#define REG_SS_NCLK			161
#define SS_NCLK_MASK		0xF0

typedef struct
{
	uint32_t SS_Amplitude_ppm;	//amplitude of the SS feature in ppm of center frequency
	EnableState SS_Enable;
	Si5351_SSModeTypeDef SS_Mode;
	Si5351_SSNCLKTypeDef SS_NCLK;
} Si5351_SSConfigTypeDef;

/*
 * This section contains data structures for configuring the
 * Output Multisynth.
 */

//this selects the Multisynth clock source
typedef enum
{
	MS_Clock_Source_PLLA = 0x00,
	MS_Clock_Source_PLLB = 0x20
} Si5351_MSClockSourceTypeDef;

#define REG_MS_P1_0_7		46
#define REG_MS_P1_8_15		45
#define REG_MS_P1_16_17		44
#define MS_P1_16_17_MASK	0x03

#define REG_MS_P2_0_7		49
#define REG_MS_P2_8_15		48
#define REG_MS_P2_16_19		47
#define MS_P2_16_19_MASK	0x0F

#define REG_MS_P3_0_7		43
#define REG_MS_P3_8_15		42
#define REG_MS_P3_16_19		47
#define MS_P3_16_19_MASK	0xF0

#define REG_MS67_P1			90

#define REG_MS_INT			16
#define MS_INT_MASK			0x40

#define REG_MS_DIVBY4		44
#define MS_DIVBY4_MASK		0x0C

#define REG_MS_SRC			16
#define MS_SRC_MASK			0x20

#define MS_SETUP_STEP		1
#define MS_DIVIDER_STEP		8

typedef struct
{
	Si5351_MSClockSourceTypeDef MS_Clock_Source; //select source on MS input
	uint32_t MS_Divider_Integer;		//the integer part of divider, called "a"
	uint32_t MS_Divider_Numerator;		//the numerator, called "b"
	uint32_t MS_Divider_Denominator;	//the denominator, called "c"
} Si5351_MSConfigTypeDef;	//sets MS divider ( a+(b/c) ) and clock (PLLA/PLLB)

/*
 * This section contains data structures for configuring the
 * CLK, R divider and output stage (joined together because they make
 * a tight block without any multiplexer).
 */

#define REG_CLK_SRC			16
#define CLK_SRC_MASK		0x0C

//this sets the CLK source clock
typedef enum
{
	CLK_Clock_Source_XTAL	 = 0x00,
	CLK_Clock_Source_CLKIN   = 0x04,
	CLK_Clock_Source_MS0_MS4 = 0x08, //this uses MS0 for CLK0..3 and MS4 for CLK4..7
	CLK_Clock_Source_MS_Own	 = 0x0C  //this uses MSx for CLKx
} Si5351_CLKClockSourceTypeDef; //configures multiplexer on CLK  input

#define REG_CLK_R_DIV		44
#define CLK_R_DIV_MASK		0x70

#define REG_CLK_R67_DIV		92
#define CLK_R67_DIV_MASK	0x07

//this sets the R divider ratio
typedef enum
{
	CLK_R_Div1 		= 0x00,
	CLK_R_Div2		= 0x10,
	CLK_R_Div4		= 0x20,
	CLK_R_Div8		= 0x30,
	CLK_R_Div16		= 0x40,
	CLK_R_Div32		= 0x50,
	CLK_R_Div64		= 0x60,
	CLK_R_Div128	= 0x70
} Si5351_CLKRDivTypeDef;

#define REG_CLK_DIS_STATE	24
#define CLK_DIS_STATE_MASK	0x03

//this sets output buffer behaviour when disabled
typedef enum
{
	CLK_Disable_State_LOW		= 0x00,
	CLK_Disable_State_HIGH		= 0x01,
	CLK_Disable_State_HIGH_Z	= 0x02,	 //three-stated when off
	CLK_Disable_State_ALWAYS_ON	= 0x03   //cannot be disabled
} Si5351_CLKDisableStateTypeDef;

#define REG_CLK_IDRV		16
#define CLK_IDRV_MASK		0x03

//this sets current drive of the output buffer
typedef enum
{
	CLK_I_Drv_2mA = 0x00,
	CLK_I_Drv_4mA = 0x01,
	CLK_I_Drv_6mA = 0x02,
	CLK_I_Drv_8mA = 0x03
} Si5351_CLKIDrvTypeDef;

#define REG_CLK_PHOFF		165
#define CLK_PHOFF_MASK		0x7F

#define REG_CLK_EN			3

#define REG_CLK_INV			16
#define CLK_INV_MASK		0x10

#define REG_CLK_PDN			16
#define CLK_PDN_MASK		0x80

#define REG_CLK_OEB			9

#define CLK_PHOFF_STEP		1
#define CLK_SETUP_STEP		1
#define CLK_R_DIV_STEP		8

typedef struct
{
	Si5351_CLKClockSourceTypeDef CLK_Clock_Source; //clock source

	/* this sets the time offset of the CLK channel, basic unit
	 * is one quarter of the VCO period (90deg offset),
	 * set it to 4*fVCO*toffset, the value is 7-bit, the max time offset
	 * varies between 35 and 53 ns (1 cycle for 28 and 19 MHz, respectively)
	 * according to the current frequency of the VCO
	 */
	uint8_t CLK_QuarterPeriod_Offset;

	Si5351_CLKRDivTypeDef CLK_R_Div; 	//R divider value (only powers of 2)
	EnableState CLK_Invert; 			//invert output clock
	EnableState CLK_Enable; 			//enable flag
	EnableState CLK_PowerDown;			//powerdown flag
	Si5351_CLKDisableStateTypeDef CLK_Disable_State; //sets output behaviour when disabled
	Si5351_CLKIDrvTypeDef CLK_I_Drv; 	//output driver current drive strength
	EnableState CLK_Use_OEB_Pin;		//allows using OEB pin to enable clock
} Si5351_CLKConfigTypeDef;

/*
 * This section contains main data structure for Si5351 configuration
 */

typedef struct
{
	/*
	 * These are frequencies of the input clocks, set it in Hz.
	 */
	uint32_t f_XTAL;
	uint32_t f_CLKIN;

	//Interrupt masking - enabling it disables the int source from pulling INTR low
	EnableState Interrupt_Mask_SysInit;
	EnableState Interrupt_Mask_PLLB;
	EnableState Interrupt_Mask_PLLA;
	EnableState Interrupt_Mask_CLKIN;
	EnableState Interrupt_Mask_XTAL;

	//Fanout enable - enables internal clock routing
	EnableState Fanout_MS_EN;
	EnableState Fanout_XO_EN;
	EnableState Fanout_CLKIN_EN;

	I2C_TypeDef *I2Cx;				//the I2C interface that will be used
	uint8_t HW_I2C_Address;			//I2C address of the Si5351 for the packages with A0 pin
									//(also, some duds with strange address reported)
	Si5351_OSCConfigTypeDef OSC;	//Oscillator, CLKIN and VCXO settings
	Si5351_PLLConfigTypeDef PLL[2];	//PLL settings for PLLA and PLLB
	Si5351_MSConfigTypeDef MS[8];	//MultiSynth[0..7] settings
	Si5351_CLKConfigTypeDef CLK[8];	//CLK[0..7], R divider and output stage settings
	Si5351_SSConfigTypeDef SS;		//spread spectrum settings
} Si5351_ConfigTypeDef;

/*
 * Typedefs for selecting PLL, MS and CLK to be used
 */

//this selects PLL channel
typedef enum
{
	PLL_A = 0,
	PLL_B = 1
} Si5351_PLLChannelTypeDef;

//this selects Multisynth channel
typedef enum
{
	MS0 = 0,
	MS1 = 1,
	MS2 = 2,
	MS3 = 3,
	MS4 = 4,
	MS5 = 5,
	MS6 = 6,
	MS7 = 7
} Si5351_MSChannelTypeDef;

//this selects CLK channel
typedef enum
{
	CLK0 = 0,
	CLK1 = 1,
	CLK2 = 2,
	CLK3 = 3,
	CLK4 = 4,
	CLK5 = 5,
	CLK6 = 6,
	CLK7 = 7
} Si5351_CLKChannelTypeDef;

//this selects device status flag
typedef enum
{
	StatusBit_SysInit = DEV_SYS_INIT_MASK,
	StatusBit_PLLA = DEV_STKY_LOL_A_MASK,
	StatusBit_PLLB = DEV_LOL_B_MASK,
	StatusBit_CLKIN = DEV_LOS_CLKIN_MASK,
	StatusBit_XTAL = DEV_LOS_XTAL_MASK,
} Si5351_StatusBitTypeDef;

//these write to and read from a Si5351 register, for porting
//purposes, these functions should be the only ones which should need edits
int Si5351_WriteRegister(Si5351_ConfigTypeDef *Si5351_ConfigStruct,  uint8_t reg_address, uint8_t reg_data);
uint8_t Si5351_ReadRegister(Si5351_ConfigTypeDef *Si5351_ConfigStruct,  uint8_t reg_address);

void Si5351_StructInit(Si5351_ConfigTypeDef *Si5351_ConfigStruct);

void Si5351_OSCConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct);

EnableState Si5351_CheckStatusBit(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_StatusBitTypeDef StatusBit);
EnableState Si5351_CheckStickyBit(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_StatusBitTypeDef StatusBit);
void Si5351_InterruptConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct);
void Si5351_ClearStickyBit(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_StatusBitTypeDef StatusBit);

void Si5351_PLLConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_PLLChannelTypeDef PLL_Channel);
void Si5351_PLLReset(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_PLLChannelTypeDef PLL_Channel);
void Si5351_PLLSimultaneousReset(Si5351_ConfigTypeDef *Si5351_ConfigStruct);

void Si5351_SSConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct);

void Si5351_MSConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_MSChannelTypeDef MS_Channel);

void Si5351_CLKPowerCmd(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_CLKChannelTypeDef CLK_Channel);
void Si5351_CLKConfig(Si5351_ConfigTypeDef *Si5351_ConfigStruct, Si5351_CLKChannelTypeDef CLK_Channel);

int Si5351_Init(Si5351_ConfigTypeDef *Si5351_ConfigStruct);
#endif /* SI5351_H_ */
