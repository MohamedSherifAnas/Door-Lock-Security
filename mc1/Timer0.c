/*************************************************************************************
 *
 * Module		: Timer0
 *
 * File Name	: Timer0.c
 *
 * Description	: Source file for AVR Timer0 driver
 *
 * Author		: Mohamed Sherif
 *
 **************************************************************************************/

#include "Timer0.h"
#include<avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/***************************************************************************************
 * 					          Function Definition	                                   *
 ***************************************************************************************/

/*
 * Description: Function to initialize the Timer0 driver
 * 	1.Initialize Timer0 mode
 * 	2.Initial start value for Timer0
 * 	3.Initial Compare value for Timer0
 * 	4.Initialize the pre-scaler value
 */
void Timer0_Init(const Timer0_ConfigType * Config_Ptr)
{
	/*
	 * Check if the timer will operate in Normal mode or Compare mode
	 * If it's normal mode put Initial value to start the timer from in TCNT0
	 * If Compare or PWM mode Put compare value in OCR0
	 */
	if(Config_Ptr->Timer0_mode == NORMAL_MODE)
	{
		/*Initial start value for Timer0*/
		TCNT0=Config_Ptr->Timer0_InitialValue;
	}
	else
	{
		/*Initial Compare value for Timer0*/
		OCR0=Config_Ptr->Timer0_InitialValue;
	}

	/*
	 * Force Output Compare
	 * FOC0 bit is active only when the timer0 mode a non-PWM mode
	 */
	TCCR0 = ( (!(Config_Ptr->Timer0_mode) & 0x01) <<FOC0);

	/*
	 * Initialize Timer0 mode by WGM00 & WGM01 bits
	 */
	TCCR0 |= (((Config_Ptr->Timer0_mode) & 0x01) <<WGM00) |((((Config_Ptr->Timer0_mode) & 0x02)>>1) <<WGM01);

	/*
	 * when working in CTC or PWM mode you can use OC0 pin
	 * we adjust the behavior of pin OC0 by COM00 & COM01 bits
	 */
	TCCR0 |=((  (Config_Ptr->OC0_PinBehavior) & 0x01) <<COM00);
	TCCR0 |=((( (Config_Ptr->OC0_PinBehavior) & 0x02) >>1) <<COM01);

	/*
	 * Initialize the pre-scaler value by using CS02 & CS01 & CS00  bits
	 */
	TCCR0 |= (((Config_Ptr->Timer0_prescaler & 0x04)>>2) <<CS02);
	TCCR0 |= (((Config_Ptr->Timer0_prescaler & 0x02)>>1) <<CS01);
	TCCR0 |= ((Config_Ptr->Timer0_prescaler & 0x01) <<CS00);

/*
 * Enable OR Disable Interrupt when  timer0 over flow occur
 */
	TIMSK |= (Config_Ptr->Over_Flow_Interrupt<<TOIE0);

	/*
	 * ENable or disable Interrupt when timer0 compare match occur
	 */
	TIMSK |= (Config_Ptr->Compare_Match_Interrupt<<OCIE0);
}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setcallback(void(*a_ptr)(void))
{
	g_callBackPtr = a_ptr;
}


void Timer0_DeInit(void)
{
	/*
	 * when CS02:CS00 =0 then timer is stopped
	 */
	TCCR0 = 0;
	TCNT0=0;
	OCR0=0;
}

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
