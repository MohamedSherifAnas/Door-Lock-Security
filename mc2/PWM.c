/********************************************************************
 *
 * Module: Timer0 PWM
 *
 * File Name: PWM.c
 *
 * Description: Source file for the Atmega PWM driver
 *
 * Author: Mohamed Sherif
 *
 *********************************************************************/

#include <avr/io.h>
#include "PWM.h"
#include "gpio.h"

/*
 *  Description:
 * The function responsible for trigger the Timer0 with the PWM Mode.
 * Setup the PWM mode with Non-Inverting.
 * Setup the prescaler with F_CPU/8.
 * Setup the compare value based on the required input duty cycle
 * Setup the direction for OC0 as output pin through the GPIO driver.
 * The generated PWM signal frequency will be 500Hz to control the DC Motor speed.
 */
void PWM_Timer2_Start(uint8 duty_cycle)

{
	TCCR2 = (1<<WGM20) |(1<<WGM21) | (1<<COM21) | (1<<CS21);

	/* Clear the timer */
	TCNT2 = 0;

	/* Setting the output compare register value */
	OCR2 = (duty_cycle*255)/100;

	/* Setup the direction for OC0 as output pin through the GPIO driver */
	GPIO_setupPinDirection(PORTB_ID,PIN3_ID,PIN_OUTPUT);

}



