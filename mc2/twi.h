/****************************************************************************************
 *
 * Module		: TWI(I2C)
 *
 * File name	:twi.h
 *
 * Description	: Header File for TWI(I2C) AVR driver
 *
 * Author		: Mohamed Sherif
 *
 *****************************************************************************************/

#ifndef TWI_H_
#define TWI_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	TWI_F_CPU_CLOCK,TWI_F_CPU_4,TWI_F_CPU_16,TWI_F_CPU_64
}TWI_prescaler;

typedef enum
{
	normal_mode=100000,fast_mode=400000,fast_mode_plus=1000000,highSpeed_mode=3400000
}TWI_bitRate;

typedef struct
{
	TWI_prescaler prescaler;
	TWI_bitRate bitRate;
}TWI_ConfigType;

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START         0x08 /* start has been sent */
#define TWI_REP_START     0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initialize the TWI device
 * Give the device an address.
 * Enable TWI
 */
void TWI_init(const TWI_ConfigType* Config_Ptr);

/*
 * Description:
 * send the start bit
 */
void TWI_start(void);

/*
 * Description:
 * send the stop bit
 */
void TWI_stop(void);

/*
 * Description:
 * to send byte
 */
void TWI_writeByte(uint8 data);

/*
 * Description:
 * to receive byte with ACK
 */
uint8 TWI_readByteWithACK(void);

/*
 * Description:
 * to receive byte with negative ACK
 */
uint8 TWI_readByteWithNACK(void);

/*
 * Description:
 * To get TWI status
 */
uint8 TWI_getStatus(void);

uint8 Calculate_power(void);

#endif /* TWI_H_ */
