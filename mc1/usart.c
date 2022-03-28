/***************************************************************************
 *
 * Module: USART
 *
 * File Name: usart.c
 *
 * Description: Source file for the USART AVR driver
 *
 * Author: Mohamed Sherif
 *
 ****************************************************************************/

#include "usart.h"
#include <avr/io.h>
#include "common_macros.h"
#include <avr/interrupt.h>



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const usart_ConfigType * Config_Ptr,uint32 baud_rate)
{
	uint16 ubrr_value = 0;

	/* U2X =0 Normal Speed OR =1 double transmission speed */
	UCSRA = (Config_Ptr->speed<<U2X);

	/************************** UCSRB Description **************************
	 * RXCIE  =0 Disable OR =1 Enable USART RX Complete Interrupt
	 * TXCIE  =0 Disable OR =1 Enable USART Tx Complete Interrupt
	 * UDRIE   	 Disable OR Enable USART Data Register Empty Interrupt
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2  	 For 8-bit data mode
	 ***********************************************************************/
	UCSRB  = (Config_Ptr->receiveInterrupt<<RXCIE) ;
	UCSRB |= (1<<RXEN) | (1<<TXEN);
	UCSRB |= (((Config_Ptr->CharacterSize & 0x04) >>2) <<UCSZ2);

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation OR = 1 Synchronous operation
	 * UPM1:0   parity bit
	 * USBS    = 0 One stop bit OR =1 Two stop bit
	 * UCSZ1:0  For data mode
	 * UCPOL    Used with the Synchronous operation only =0
	 ***********************************************************************/
	UCSRC  = (1<<URSEL) |(Config_Ptr->modeSelect<<UMSEL);

	/*
	 * Masking to get the value of UPM0 bit from 2 bit number
	 * Masking to get the value of UPM1 bit from 2 bit number
	 */
	UCSRC |= ((Config_Ptr->ParityMode &0x01)<<UPM0) |(((Config_Ptr->ParityMode &0x02)>>1) <<UPM1);
	UCSRC |= (Config_Ptr->stopBitSelect<<USBS);
	/*
	 * Masking to take the first bit in CharacterSize and write it in UCSZ0
	 * Masking to take the Second bit in CharacterSize and write it in UCSZ1
	 * To Configure the number of bit you want to send or receive
	 */
	UCSRC |= ((Config_Ptr->CharacterSize & 0x01)<<UCSZ0) | (((Config_Ptr->CharacterSize & 0x02)>>1) <<UCSZ1);
	UCSRC |=  (Config_Ptr->Edge <<UCPOL);

	/* Calculate the UBRR register value */
	ubrr_value = (uint16)(((F_CPU / (baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
	return UDR;
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}

/*********************************************************************************
 * 								ISR's Code		    							 *
 *********************************************************************************/
ISR(USART_RXC_vect)
{
	g_recieved_message=UART_recieveByte();


}
