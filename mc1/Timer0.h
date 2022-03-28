/********************************************************************************************
 *
 * Module		: Timer0
 *
 * File Name	: Timer0.h
 *
 * Description	: Header file for AVR Timer0 driver
 *
 * Author		: Mohamed Sherif
 *
 *********************************************************************************************/

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"
#include<avr/io.h>

/*********************************************************************************
 * 					          Deceleration for user defined types	             *
 *********************************************************************************/
typedef enum
{
	NORMAL_MODE,PWM_MODE,CTC_MODE,FAST_PWM_MODE
}Timer0_mode;

typedef enum
{
	Timer0_NO_CLOCK,Timer0_F_CPU_CLOCK,Timer0_F_CPU_8,Timer0_F_CPU_64,Timer0_F_CPU_256,Timer0_F_CPU_1024
}Timer0_prescaler;

typedef enum
{
	NORMAL_OC0_DISCONNECTED,TOGGLE_OC0_ON_COMPARE,CLEAR_OC0_ON_COMPARE,SET_OC0_ON_COMPARE
}Timer0_OC0_PinBehavior;

typedef enum
{
	Over_Flow_Interrupt_Disable,Over_Flow_Interrupt_Enable
}Timer0_Over_Flow_Interrupt_Enable;

typedef enum
{
	Compare_Match_Interrupt_Disable,Compare_Match_Interrupt_Enable
}Timer0_Compare_Match_Interrupt_Enable;

typedef struct
{
	Timer0_mode Timer0_mode;
	uint8 Timer0_InitialValue;
	uint8 Timer0_CombareValue;
	Timer0_OC0_PinBehavior OC0_PinBehavior;
	Timer0_prescaler Timer0_prescaler;
	Timer0_Over_Flow_Interrupt_Enable Over_Flow_Interrupt;
	Timer0_Compare_Match_Interrupt_Enable Compare_Match_Interrupt;
}Timer0_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description: Function to initialize the Timer0 driver
 * 	1.Initialize Timer0 mode
 * 	2.Initial start value for Timer0
 * 	3.Initial Compare value for Timer0
 * 	4.Initialize the pre-scaler value
 */
void Timer0_Init(const Timer0_ConfigType * Config_Ptr);

/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setcallback(void(*a_ptr)(void));

/*
 * Description: Function to disable the Timer0
 */
void Timer0_DeInit(void);

#endif /* TIMER0_H_ */
