/************************************************************************
 *
 * Module: DC-Motor
 *
 * File Name: DC-motor.c
 *
 * Description: the source file for the Atmega DC motor driver
 *
 * Author: Mohamed Sherif
 *
 ************************************************************************/

#include "DC-motor.h"
#include "gpio.h"
#include "PWM.h"

/*
 * Description:
 * the Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void)
{
	/* setup the direction for the two motor pins through the GPIO driver*/
	GPIO_setupPinDirection(Motor_PORT_ID,Motor_PIN1_ID,PIN_OUTPUT);

	GPIO_setupPinDirection(Motor_PORT_ID,Motor_PIN2_ID,PIN_OUTPUT);

	/*Stop at the DC-Motor at the beginning through the GPIO driver */
	GPIO_writePin(Motor_PORT_ID,Motor_PIN1_ID,LOGIC_LOW);

	GPIO_writePin(Motor_PORT_ID,Motor_PIN2_ID,LOGIC_LOW);
}

/*
 * Description:
 *  The function responsible for rotate the DC Motor CW/ or A-CW or
 *  stop the motor based on the state input state value.
 *   Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	switch (state)
	{
	case OFF:
		GPIO_writePin(Motor_PORT_ID,Motor_PIN1_ID,LOGIC_LOW);
		GPIO_writePin(Motor_PORT_ID,Motor_PIN2_ID,LOGIC_LOW);
		break;

	case CW:
		GPIO_writePin(Motor_PORT_ID,Motor_PIN1_ID,LOGIC_HIGH);
		GPIO_writePin(Motor_PORT_ID,Motor_PIN2_ID,LOGIC_LOW);
		PWM_Timer2_Start(speed);
		break;

	case CCW:
		GPIO_writePin(Motor_PORT_ID,Motor_PIN1_ID,LOGIC_LOW);
		GPIO_writePin(Motor_PORT_ID,Motor_PIN2_ID,LOGIC_HIGH);
		PWM_Timer2_Start(speed);
		break;

	default:
		GPIO_writePin(Motor_PORT_ID,Motor_PIN1_ID,LOGIC_LOW);
		GPIO_writePin(Motor_PORT_ID,Motor_PIN2_ID,LOGIC_LOW);
		break;

	}

}
