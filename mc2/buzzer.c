/********************************************************************************
 *
 * Module		: Buzzer
 *
 * File Name	: buzzer.c
 *
 * Descriptions	: source file for buzzer driver
 *
 * Author: Mohamed Sherif
 *
 *******************************************************************************/

#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                                Function Definitions                         *
 *******************************************************************************/

/*
 * Description:
 * initialize the buzzer pin as output pin
 */
void buzzer_Init(void)
{
	GPIO_setupPinDirection(PORT_NUM, PIN_NUM, PIN_OUTPUT);
}

/*
 * Description:
 * turn on the buzzer
 */
void buzzer_on(void)
{
	/*
	 * turn on the buzzer by making pin value Logic High=1
	 */
	GPIO_writePin(PORT_NUM, PIN_NUM, LOGIC_HIGH);
}

/*
 * Description:
 * turn off the buzzer
 */
void buzzer_off(void)
{
	/*
	 * turn OFF the buzzer by making pin value Logic Low=0
	 */
	GPIO_writePin(PORT_NUM, PIN_NUM, LOGIC_LOW);
}
