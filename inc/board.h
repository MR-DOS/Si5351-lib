/*
 * board.h
 *
 *  Created on: Dec 24, 2017
 *      Author: msboss
 */

#ifndef BOARD_H_
#define BOARD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define LED_BACK	GPIO_Pin_2
#define LED_OK		GPIO_Pin_1
#define LED_LEFT	GPIO_Pin_3
#define LED_RIGHT	GPIO_Pin_4
#define LED_UP		GPIO_Pin_6
#define LED_DOWN	GPIO_Pin_0

#define BTN_BACK	LED_BACK
#define BTN_OK		LED_OK
#define BTN_LEFT	LED_LEFT
#define BTN_RIGHT	LED_RIGHT
#define BTN_UP		LED_UP
#define BTN_DOWN	LED_DOWN

#define SDA			GPIO_Pin_10
#define SCL			GPIO_Pin_9
#define SDA_PinSource	GPIO_PinSource9
#define SCL_PinSource	GPIO_PinSource10

typedef enum {DISABLED=0, ENABLED=1} ForceState;

#ifndef ENABLESTATE
#define ENABLESTATE
	typedef enum
	{
		OFF = 0,
		ON = 1
	} EnableState;
#endif

#include "main.h"

void SysTick_Handler(void);
void Delay_ms(uint32_t Wait);

void I2C_Board_Init(ForceState Force_Init);
void I2C_Board_Deinit(ForceState Force_Deinit);

void Init_Board(void);

void Button_Init(void);
void Button_Deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
