/*******************************************************************************
 *
 * Module		: Buzzer
 *
 * File name	: buzzer.h
 *
 * Description	: Header file for buzzer driver
 *
 * Author		: Mohamed Sherif
 *
 *********************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                                Common Header Files                          *
 *******************************************************************************/
#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                Static configurations                        *
 *******************************************************************************/
#define PORT_NUM		PORTD_ID
#define PIN_NUM			PIN2_ID

/*******************************************************************************
 *                                Function Prototypes                          *
 *******************************************************************************/

/*
 * Description:
 * initialize the buzzer pin as output pin
 */
void buzzer_Init(void);

/*
 * Description:
 * turn on the buzzer
 */
void buzzer_on(void);

/*
 * Description:
 * turn off the buzzer
 */
void buzzer_off(void);

#endif /* BUZZER_H_ */
