/************************************************************************
 *
 * Module: USART
 *
 * File Name: usart.h
 *
 * Description: Header file for the USART AVR driver
 *
 * Author: Mohamed Sherif
 *
 *************************************************************************/

#ifndef USART_H_
#define USART_H_

#include "std_types.h"

#define NEW_PASSWORD 		0x01
#define SAVE_PASSWORD   	0x02
#define CHANGE_PASSWORD 	0x03
#define OPEN_DOOR  			0x04
#define CORRECT_PASSWORD 	0x05
#define WRONG_PASSWORD		0x06
#define error 				0x07
#define DONT_CHANGE 		0x08

/************************************************************************
 *                           Global variables                           *
 ************************************************************************/
extern uint8 g_recieved_message;
extern uint8 g_Pass_recieved[5];


/*******************************************************************************
 *                       User Defined Types                                    *
 *******************************************************************************/
typedef enum
{
	NORMAL_SPEED,DOUBLE_SPEED
}usart_speedTransmission;

typedef enum
{
	DISABLE_RX,ENABLE_RX
}usart_receive_Enable_Interrupt;

typedef enum
{
	DISABLE_TX,ENABLE_TX
}usart_transmit_Enable_Interrupt;

typedef enum
{
	FIVE_BIT,SIX_BIT,SEVEN_BIT,EIGHT_BIT,NINE_BIT=7
}usart_CharacterSize;

typedef enum
{
	ASYNCHRONOUS,SYNCHRONOUS
}usart_modeSelect;

typedef enum
{
	DISABLE_PARITY,RESERVED,ENABLE_EVEN_PARITY,ENABLE_ODD_PARITY
}usart_parityMode;

typedef enum
{
	ONE_BIT,TWO_BIT
}usart_stopBitSelect;

typedef enum
{
	RISING,FALLING
}usart_clockPolarity;

typedef struct
{
	usart_speedTransmission speed;
	usart_receive_Enable_Interrupt receiveInterrupt;
	usart_transmit_Enable_Interrupt transmitInterrupt;
	usart_CharacterSize CharacterSize;
	usart_modeSelect modeSelect;
	usart_parityMode ParityMode;
	usart_stopBitSelect stopBitSelect;
	usart_clockPolarity Edge;

}usart_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const usart_ConfigType * Config_Ptr,uint32 baud_rate);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* USART_H_ */
