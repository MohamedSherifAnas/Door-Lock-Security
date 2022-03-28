/*
 ============================================================================================
  Name			: MC1.c
  Author		: Mohamed Sherif
  Description	: Soucre Code for the HMI-ECV
  Date			:6/11/2021
  ===========================================================================================
 */
#include"lcd.h"
#include "KeyPad.h"
#include"usart.h"
#include"Timer0.h"
#include "util/delay.h"



#define F_CPU 8000000

/* Number of required interrupt to give you the time you want
 * i used another number because simulation not accurate
 * the actual calculated number 465
 */
#define NUMBER_OVER_FLOW_ROTATE 	55
/*the actual calculated number 93 */
#define NUMBER_OVER_FLOW_HOLD 		15
/*the actual calculated number 1860 */
#define NUMBER_OVER_FLOW_BUZZER 	250

#define Password_Number 			5

/************************************************************************
 *                           Global variables                           *
 ************************************************************************/

/* Flag to indicate the first or second time to use the program */
uint8 g_First_use=0;

/* Array to Holds the first entered password */
uint8 g_a_firstPass[Password_Number];

/* Array to Holds the second entered password */
uint8 g_a_secondPass[Password_Number];

/* variable to hold the return value of "compare" function when comparing 2 password */
uint8 g_compare = 0;

/* variable to hold the recieved message from the Control-ECV */
uint8 g_recieved_message;

/* Array to store in it the password required to send */
uint8 g_a_Pass_send[6];

/* Flag to indicate when the timer finish interrupts */
uint8 g_interrupt_flag=1;

/* Counter to count number of interrupt in ISR */
uint16 g_timercounter;

/* Variable to store in it the key pressed from the key pad */
uint8 g_keyPressed;

/************************************************************************
 *                          Function Prototype                          *
 ************************************************************************/

/*
 * Description:
 * 1.function get the password from the user and store it in global array
 * 2. display '*' when the user press key on the key pad
 */
void getPassword(uint8 array[]);

/*
 * Description:
 * Function compare two array and if they are different increment the count
 */
uint8 compareTwoPasswords(uint8 arr1[], uint8 arr2[]);

/*
 * Description:
 * Function prepare the array to be send by the UART by adding '#'
 */
void prepareToSend(uint8 array[], uint8 array2[]);

/*
 * Description:
 * Timer call back function when the motor are rotating
 */
void callBackFunction_rotateMotor ();

/*
 * Description:
 * Timer call back function when the motor are holding
 */
void callBackFunction_holdMotor ();

/*
 * Description:
 * Timer call back function when the buzzer are on
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
	 * Description :
	 * Initialize the LCD:
	 * 1. Setup the LCD pins directions by use the GPIO driver.
	 * 2. Setup the LCD Data Mode 8-bits.
	 */
	LCD_init();


	while(1)
	{
		if(g_First_use==0)
		{
			/*
			 * 1.Clear the LCD
			 * 2.Display the required Message
			 * 3.Move the cursor to the location where we type the password
			 */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Enter New Pass:");
			LCD_moveCursor(1, 0);

			/*
			 * Function to get the first password entered and store it in Array
			 */
			getPassword(g_a_firstPass);

			/*
			 * 1.Clear the LCD
			 * 2.Display the required Message
			 * 3.Move the cursor to the location where we type the password
			 */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Renter Pass:");
			LCD_moveCursor(1, 0);

			/*
			 * Function to get the Second password entered and store it in Array
			 */
			getPassword(g_a_secondPass);

			/*
			 * Check if the first password matches with the second one
			 */
			g_compare=compareTwoPasswords(g_a_firstPass, g_a_secondPass);
			if (g_compare == 0)
			{
				/* Make the flag equal one to go to the Main option */
				g_First_use=1;

				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 4, "Password");
				LCD_displayStringRowColumn(1, 5, "Match");
				/* delay to display the required message */
				_delay_ms(1000);

				prepareToSend(g_a_firstPass, g_a_Pass_send);

				UART_sendByte(SAVE_PASSWORD);
				UART_sendString(g_a_Pass_send);
			}
			else
			{
				/* Clear the flag to go to the First step again */
				g_First_use=0;

				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 0, "Pass Not Match");
				LCD_displayStringRowColumn(1, 0, "Please ReEnter");

				/* delay to display the required message */
				_delay_ms(1000);
			}
		}

		else
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "+ : Open Door   ");
			LCD_displayStringRowColumn(1, 0, "- : Change Pass");

			g_keyPressed=KEYPAD_getPressedKey();

			if(g_keyPressed=='+')
			{
				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 0,"Enter Pass:");
				LCD_moveCursor(1, 0);
				/* delay to display the required message */
				_delay_ms(500);

				getPassword(g_a_firstPass);
				prepareToSend(g_a_firstPass, g_a_Pass_send);

				UART_sendByte(OPEN_DOOR);
				UART_sendString(g_a_Pass_send);

				/* delay until the second Micro send message before entering the switch case */
				_delay_ms(500);
			}
			else if(g_keyPressed=='-')
			{

				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 0,"Enter Pass:");
				LCD_moveCursor(1, 0);
				/* delay to display the required message */
				_delay_ms(500);

				getPassword(g_a_firstPass);
				prepareToSend(g_a_firstPass, g_a_Pass_send);

				UART_sendByte(CHANGE_PASSWORD);
				UART_sendString(g_a_Pass_send);

				/* delay until the second Micro send message before entering the switch case */
				_delay_ms(500);
			}

			switch(g_recieved_message)
			{
			/* in every case must clear the g_recieved_message to enter the switch case one time only
			 *  and then stay in default
			 */
			case CORRECT_PASSWORD:

				LCD_clearScreen();
				LCD_displayString("CORRECT_PASSWORD");
				/* delay to display the required message */
				_delay_ms(400);

				Timer0_setcallback(callBackFunction_rotateMotor);
				Timer0_Init(&Timer0_Config);

				LCD_clearScreen();
				LCD_displayString("Door Unlocking");

				/* loop until the timer0 complete all interrupts */
				while(g_interrupt_flag !=0){}
				g_interrupt_flag=1;
				Timer0_DeInit();


				Timer0_setcallback(callBackFunction_holdMotor);
				Timer0_Init(&Timer0_Config);

				LCD_clearScreen();
				LCD_displayString("Holding Door");

				while(g_interrupt_flag !=0){}
				g_interrupt_flag=1;
				Timer0_DeInit();


				Timer0_setcallback(callBackFunction_rotateMotor);
				Timer0_Init(&Timer0_Config);

				LCD_clearScreen();
				LCD_displayString("Door locking");

				while(g_interrupt_flag !=0){}
				g_interrupt_flag=1;
				Timer0_DeInit();

				g_recieved_message=0;

				break;

			case WRONG_PASSWORD:

				LCD_clearScreen();
				LCD_displayString("WRONG_PASSWORD");
				_delay_ms(1000);

				g_recieved_message=0;

				break;

			case NEW_PASSWORD:

				g_First_use=0;
				g_recieved_message=0;

				break;

			case error:

				LCD_clearScreen();
				LCD_displayString("ERROR");

				Timer0_setcallback(callBackFunction_buzzer);
				Timer0_Init(&Timer0_Config);
				while(g_interrupt_flag !=0){}
				g_interrupt_flag=1;
				Timer0_DeInit();

				g_recieved_message=0;

				break;

			default:
				break;
			}
		}
	}
}

/*
 * Description:
 * 1.function get the password from the user and store it in global array
 * 2. display '*' when the user press key on the key pad
 */
void getPassword(uint8 array[])
{
	/* counter to access the array */
	uint8 i;

	/*variable to store key pressed in Array*/
	uint8 key=0;

	for(i=0;i<5;i++)
	{

		key=KEYPAD_getPressedKey();
		if(key==13)
		{
			array[i]='\0';

		}
		else
		{
			array[i]=key;

			LCD_displayCharacter('*');
		}
		_delay_ms(400);
	}

}

/*
 * Description:
 * Function compare two array and if they are different increment the count
 */
uint8 compareTwoPasswords(uint8 arr1[], uint8 arr2[])
{
	uint8 count = 0;
	sint8 i;
	for (i = 0; i < 5; i++)
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
 * Function prepare the array to be send by the UART by adding '#'
 */
void prepareToSend(uint8 array[], uint8 array2[])
{
	uint8 i = 0;
	while(array[i]!='\0')
	{
		array2[i] = array[i];
		i++;
	}

	array2[i] = '#';
	i++;
	array2[i] = '\0';
	//break;
}

/*
 * Description:
 * Timer call back function when the motor are rotating
 */
void callBackFunction_rotateMotor ()
{
	g_timercounter++;
	if(g_timercounter == NUMBER_OVER_FLOW_ROTATE)
	{
		g_interrupt_flag=0;
		g_timercounter=0;
	}
}

/*
 * Description:
 * Timer call back function when the motor are holding
 */
void callBackFunction_holdMotor ()
{
	g_timercounter++;
	if(g_timercounter == NUMBER_OVER_FLOW_HOLD)
	{
		g_interrupt_flag=0;
		g_timercounter=0;
	}
}

/*
 * Description:
 * Timer call back function when the buzzer are on
 */
void callBackFunction_buzzer ()
{
	g_timercounter++;
	if(g_timercounter == NUMBER_OVER_FLOW_BUZZER)
	{
		g_interrupt_flag=0;
		g_timercounter=0;
	}
}
