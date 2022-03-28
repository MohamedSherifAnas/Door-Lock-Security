/*
 ============================================================================================
  Name			: MC2.c
  Author		: Mohamed Sherif
  Description	: The source file for the Control-ECV
  Date			:6/11/2021
  ===========================================================================================
 */

#include"twi.h"
#include"external_eeprom.h"
#include"util/delay.h"
#include"Timer0.h"
#include"buzzer.h"
#include"usart.h"
#include"DC-motor.h"


#define F_CPU 8000000

/* Number of required interrupt to give you the time you want
 * i used another number because simulation not accurate
 * the actual calculated number 465
 */
#define NUMBER_OVER_FLOW_ROTATE 55
/*the actual calculated number 93 */
#define NUMBER_OVER_FLOW_HOLD 15
/*the actual calculated number 1860 */
#define NUMBER_OVER_FLOW_BUZZER 250

#define NUMBER_PASSWORD 5

/************************************************************************
 *                           Global variables                           *
 ************************************************************************/
/* Variable to store received message from the HMI-ECV */
uint8 g_recieved_message;

/* Array to store the received password */
uint8 g_Pass_recieved[NUMBER_PASSWORD];

/* Array used to get the password from the EEPROM and store it */
uint8 g_password_eeprom[4];

/* Variable to count number of Mis-match password */
uint8 g_trialNumber;
/* Variable to store the compare value returned from the compare function */
uint8 g_compare_value;

/* Flag to stay in the while loop until the timer finish all interrupt */
uint8 interrupt_flag=1;

/* Flag to count the number of  required interrupt */
uint16 timercounter;

/************************************************************************
 *                          Function Prototype                          *
 ************************************************************************/
/*
 * Description:
 * Function to take the password and store it in the EEPROM
 */
void storeNewPassword (uint8 array[]);

/*
 * Description:
 * Function to get the stored password in the EEPROM and store it in Array
 */
void ReadPassword ();

/*
 * Description:
 * Function compare two array stored in them two different password
 */
uint8 compareTwoPasswords(uint8 arr1[], uint8 arr2[]);

/*
 * Description:
 * function to clear an array
 */
void clearArray(uint8 *a_Ptr);

/*
 * Description:
 * Rotating motor call back function
 */
void callBackFunction_rotateMotor ();

/*
 * Description:
 * Holding Motor call back function
 */
void callBackFunction_holdMotor ();

/*
 * Description:
 * Buzzer call back Function
 */
void callBackFunction_buzzer ();


int main(void)
{
	/* Enable I-Bit for Interrupts*/
	SREG |=(1<<7);

	/*
	 * Initialize the UART:
	 * 1.Double speed mode
	 * 2.Enable receive interrupt
	 * 3.send and receive 8-bit data
	 * 4.Asynchronous
	 * 5.Disable Parity
	 * 6.one stop bit
	 */
	usart_ConfigType usart_Config;
	usart_Config.speed=DOUBLE_SPEED;
	usart_Config.receiveInterrupt=ENABLE_RX;
	usart_Config.CharacterSize=EIGHT_BIT;
	usart_Config.modeSelect=ASYNCHRONOUS;
	usart_Config.ParityMode=DISABLE_PARITY;
	usart_Config.stopBitSelect=ONE_BIT;

	/* Initialize the UART driver with Baud-rate = 9600 bits/sec */
	UART_init(&usart_Config, 9600);

	/*
	 * Initialize the Timer0:
	 * 1.Over flow mode
	 * 2.Timer0 InitialValue=0
	 * 3.Timer0 prescaler=1024
	 * 4.Over Flow Interrupt Enabled
	 */
	Timer0_ConfigType Timer0_Config;
	Timer0_Config.Timer0_mode=NORMAL_MODE;
	Timer0_Config.Timer0_InitialValue=0;
	Timer0_Config.Timer0_CombareValue=0;
	Timer0_Config.OC0_PinBehavior=NORMAL_OC0_DISCONNECTED;
	Timer0_Config.Timer0_prescaler=Timer0_F_CPU_1024;
	Timer0_Config.Over_Flow_Interrupt=Over_Flow_Interrupt_Enable;
	Timer0_Config.Compare_Match_Interrupt=Compare_Match_Interrupt_Disable;

	/*
	 * Initialize the TWI:
	 * 1.same F_CPU
	 * 2.fast_mode 400 Kbs
	 */
	TWI_ConfigType TWI_Config={TWI_F_CPU_CLOCK,fast_mode};
	TWI_init(&TWI_Config);

	/*Initialize the DcMotor */
	DcMotor_Init();

	/*Initialize the Buzzer */
	buzzer_Init();

	while(1)
	{

		switch (g_recieved_message)
		{
		/* in every case must clear the g_recieved_message to enter the switch case one time only
		 *  and then stay in default
		 */
		case SAVE_PASSWORD:

			/* store the password in the EEPROM */
			storeNewPassword(g_Pass_recieved);

			/* Clear Array that receive the password to be ready to receive new value */
			clearArray(g_Pass_recieved);

			g_recieved_message=0;

			break;

		case OPEN_DOOR:

			/* Read password from the EEPROM and Store it in array g_password_eeprom */
			ReadPassword();

			g_compare_value=compareTwoPasswords(g_Pass_recieved, g_password_eeprom);
			if(g_compare_value == 0)
			{
				g_trialNumber=0 ;
				/*
				 * Sending Message to the other micro All message included in the usart.h file
				 */
				UART_sendByte(CORRECT_PASSWORD) ;

				_delay_ms(500);
			}
			else
			{
				g_trialNumber++ ;
				if (g_trialNumber ==3)
				{
					UART_sendByte(error) ;

					_delay_ms(500);

					Timer0_setcallback(callBackFunction_buzzer);
					Timer0_Init(&Timer0_Config);
					buzzer_on();
					/* Loop to stay in it until the timer finish interrupts */
					while(interrupt_flag !=0){}
					/* Clear make the flag equal 1 to ready for the next loop */
					interrupt_flag=1;
					Timer0_DeInit();
					buzzer_off();

					g_trialNumber =0;
					g_recieved_message=0;
					break;
				}

				UART_sendByte(WRONG_PASSWORD);

				_delay_ms(500);

				g_recieved_message=0;
				break;
			}

			/* Rotate the motor clock wise with 50% of its speed */
			DcMotor_Rotate(CW, 50);

			/* start the timer and wait until it count the required time */
			Timer0_setcallback(callBackFunction_rotateMotor);
			Timer0_Init(&Timer0_Config);

			while(interrupt_flag !=0){}
			interrupt_flag=1;
			Timer0_DeInit();

			DcMotor_Rotate(OFF, 0);

			Timer0_setcallback(callBackFunction_holdMotor);
			Timer0_Init(&Timer0_Config);

			while(interrupt_flag !=0){}
			interrupt_flag=1;
			Timer0_DeInit();

			DcMotor_Rotate(CCW, 50);

			Timer0_setcallback(callBackFunction_rotateMotor);
			Timer0_Init(&Timer0_Config);

			while(interrupt_flag !=0){}
			interrupt_flag=1;
			Timer0_DeInit();

			DcMotor_Rotate(OFF, 0);

			g_recieved_message=0;

			break;

		case CHANGE_PASSWORD:

			ReadPassword();

			g_compare_value=compareTwoPasswords(g_Pass_recieved, g_password_eeprom);
			if(g_compare_value == 0)
			{
				UART_sendByte(NEW_PASSWORD);

			}
			else
			{
				g_trialNumber++ ;
				if (g_trialNumber ==3)
				{
					UART_sendByte(error) ;

					_delay_ms(500);

					Timer0_setcallback(callBackFunction_buzzer);
					Timer0_Init(&Timer0_Config);

					buzzer_on();

					while(interrupt_flag !=0){}
					interrupt_flag=1;
					Timer0_DeInit();

					buzzer_off();

					g_trialNumber =0;
					g_recieved_message=0;

					break;
				}
				UART_sendByte(WRONG_PASSWORD);

				_delay_ms(500);

				g_recieved_message=0;

				break;
			}

			g_recieved_message=0;

			break;

		default:
			break;
		}
	}
}

/*
 * Description:
 * Function to take the password and store it in the EEPROM
 */
void storeNewPassword (uint8 array[])
{
	uint8 index=0 ;
	/* address of the eeprom */
	uint16 add=0x0300;
	while(array[index] != '\0')
	{
		/* Write in the external EEPROM */
		EEPROM_writeByte(add, array[index]);
		/* delay time as the eeprom take 10ms to make a write*/
		_delay_ms(10) ;
		add++ ;
		index++ ;
	}
}
/*
 * Description:
 * Function to get the stored password in the EEPROM and store it in Array
 */
void ReadPassword ()
{
	uint8 counter = 0;
	uint16 i2cAddress=0x0300;
	while(counter < 4)
	{
		EEPROM_readByte(i2cAddress, &g_password_eeprom[counter]);
		counter++;
		i2cAddress++;
	}
	/*	LCD_intgerToString(g_password_eeprom[0]);

	LCD_intgerToString(g_password_eeprom[1]);

	LCD_intgerToString(g_password_eeprom[2]);

	LCD_intgerToString(g_password_eeprom[3]);*/
}

/*
 * Description:
 * Function compare two array stored in them two different password
 */
uint8 compareTwoPasswords(uint8 arr1[], uint8 arr2[])
{
	uint8 count = 0;
	sint8 i;
	for (i = 0; i < 4; i++)
	{
		if (arr1[i] != arr2[i])
		{
			count++;
		}
	}
	return count;
}

/*
 * Description:
 * function to clear an array
 */
void clearArray(uint8 *a_Ptr)
{
	uint8 index=0;
	while(a_Ptr[index]!='\0')
	{
		a_Ptr[index]=0;
	}
	a_Ptr[index]=0;
}


/*
 * Description:
 * Rotating motor call back function
 */
void callBackFunction_rotateMotor ()
{
	timercounter++;
	if(timercounter == NUMBER_OVER_FLOW_ROTATE)
	{
		interrupt_flag=0;
		timercounter=0;
	}
}

/*
 * Description:
 * Holding Motor call back function
 */
void callBackFunction_holdMotor ()
{
	timercounter++;
	if(timercounter == NUMBER_OVER_FLOW_HOLD)
	{
		interrupt_flag=0;
		timercounter=0;
	}
}
/*
 * Description:
 * Buzzer call back Function
 */
void callBackFunction_buzzer ()
{
	timercounter++;
	if(timercounter == NUMBER_OVER_FLOW_BUZZER)
	{
		interrupt_flag=0;
		timercounter=0;
	}
}


