/**************************************************************************
 *
 * Module		: TWI(I2C)
 *
 * File Name	: twi.c
 *
 * Description	: Source file for the TWI(I2C) AVR driver
 *
 *  Author		: Mohamed Sherif
 *
 ******************************************************************************/

#include "twi.h"

#include "common_macros.h"
#include <avr/io.h>

/*******************************************************************************
 *                     Global Variables   	                                   *
 *******************************************************************************/
uint8 g_prescaler;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the TWI device
 * Give the device an address.
 * Enable TWI
 */
void TWI_init(const TWI_ConfigType* Config_Ptr)
{

	g_prescaler=Config_Ptr->prescaler;

	/*
	 * TWPS: TWI prescaler Bits
	 */
	TWSR=(Config_Ptr->prescaler);

	/*
	 * TWBR register used in generating SCL frequency while  operating in master mode
	 * SCL frequency = CPU Frequency/(16+(2 * TWBR * (4 power TWPS) ) )
	 * SCL frequency = Chosen BitRate
	 * the equation is used to get TWBR Register Value
	 */
	TWBR=( (F_CPU/(Config_Ptr->bitRate)) -16)/(2 * Calculate_power() );

	/* Two Wire Bus address my address if any master device want to call me: 0x1 (used in case this MC is a slave device)
       General Call Recognition: Off */
	TWAR = 0b00000010; // my address = 0x01 :)

	TWCR=(1<<TWCR); /* enable TWI */
}

/*
 * Description:
 * send the start bit
 */
void TWI_start(void)
{
	/*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the start bit by TWSTA=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1<<TWINT) |(1<<TWSTA) |(1<<TWEN);

	/* Wait for TWINT flag set in TWCR Register (start bit is send successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*
 * Description:
 * send the stop bit
 */
void TWI_stop(void)
{
	/*
	 * Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/*
 * Description:
 * to send byte
 */
void TWI_writeByte(uint8 data)
{
	/* Put data On TWI data Register */
	TWDR = data;

	/*
	 * Clear the TWINT flag before sending the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);

	/* Wait for TWINT flag set in TWCR Register(data is send successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*
 * Description:
 * to receive byte with ACK
 */
uint8 TWI_readByteWithACK(void)
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable sending ACK after reading or receiving data TWEA=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	/* Wait for TWINT flag set in TWCR Register (data received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));

	/* Read Data */
	return TWDR;
}

/*
 * Description:
 * to receive byte with negative ACK
 */
uint8 TWI_readByteWithNACK(void)
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);

	/* Wait for TWINT flag set in TWCR Register (data received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
	/* Read Data */
	return TWDR;
}

/*
 * Description:
 * To get TWI status
 */
uint8 TWI_getStatus(void)
{
	uint8 status;

	/* masking to eliminate first 3 bits and get the last 5 bits (status bits) */
	status = TWSR & 0xF8;
	return status;
}

/*
 * Description:
 * Calculate the power in the SCL frequency equation "4 power TWPS value"
 * to know the value to put in  TWBR register
 */
uint8 Calculate_power(void)
{

	uint8 power_value=1,i;
	for(i=0; i<g_prescaler ;i++)
	{
		power_value=power_value * 4;
	}
	return power_value;
}
