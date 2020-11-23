/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 15 SEP 2020                                */
/* Version   : V01                                        */
/**********************************************************/

#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "RCC_interface.h"
#include "GPIO_interface.h"
#include "UART_interface.h"
#include "ESP_interface.h"
#include "FPEC_interface.h"
#include "PARSE_interface.h"

volatile u8  u8RecBuffer[1500];
volatile u8  u8FinalBuffer[50];
volatile u16 u16RecLength = 0;
volatile u16 u16RecIndex = 0;
volatile u8  u8CheckSum  = 0;
volatile u8  u8BLWriteReq = 1;
typedef void (*Function_t)(void);
Function_t addr_to_call = 0;

void func(void)
{
	/* Define the vector table register */
    #define SCB_VTOR   *((volatile u32*)0xE000ED08)

	/* The first address of the application part 
	 * which is the address of the vector table
	 */
	SCB_VTOR = 0x08002000;

	/* Cast the address as a pointer to function 
	 * and make a dereference(get what inside this address) */
	addr_to_call = *(Function_t*)(0x08002004);
	/* Call the startup code of the application */
	addr_to_call();
}

void main(void)
{
	volatile u8 Local_u8RecStatus = 0;
	volatile u8 Local_u8flag = 0;
	u8 j = 0;
	RCC_voidInitSysClock();
	RCC_voidEnableClock(RCC_APB2,14); /* USART1 */
	RCC_voidEnableClock(RCC_APB2,2);  /* PortA  */
	RCC_voidEnableClock(RCC_AHB,4);   /* FPEC   */


	/* A9 = TX output 50 Mhz  Note : You must configure this pin as AFPP  */
	GPIO_voidSetPinDirection(GPIOA , PIN9 , OUTPUT_SPEED_50MHZ_AFPP);

	/* A10 = RX input floating */
	GPIO_voidSetPinDirection(GPIOA , PIN10 , INPUT_FLOATING);
	MUART1_voidInit();// Initialize UART1
    HESP_voidInit();// Initialize ESP

	/* Make the microcontroller connected with WIFI */
    HESP_voidConnectOnWiFi("Medhat","05011969Mg");

    HESP_voidConnectOnServer("TCP" , "162.253.155.226" , "80");
   	Local_u8RecStatus = HESP_u8SendRequest("GET http://otaprog.freevar.com/TXT_Folder/AppFlag.txt" , &u8RecBuffer , "55" , &u16RecLength , &u16RecIndex);
   	Local_u8flag = u8RecBuffer[u16RecIndex];

   	if(Local_u8flag == '1')
   	{
		while(1)
		{
			/* Connect the microcontroller with the website */
			HESP_voidConnectOnServer("TCP" , "162.253.155.226" , "80");
			/* To make syncronization between the microcontroller and
			 * the website we use flag.txt file
			 * The server will send 30 records saved in the file
			 * after flashing the records we need another records,
			 * so this file indicates if there are new records come
			 * or not
			 */
			Local_u8RecStatus = HESP_u8SendRequest("GET http://otaprog.freevar.com/TXT_Folder/flag.txt" , &u8RecBuffer , "52" , &u16RecLength , &u16RecIndex);  //GET http://otaarmnedaa.freevar.com/flag.txt , length = 45   //HESP_u8SendRequest("GET http://otaarm-001-site1.itempurl.com/flag.txt" , &Local_u8flag , "51");
			/* Pass what inside the flag.txt file to Local_u8flag */
			Local_u8flag = u8RecBuffer[u16RecIndex];
			/* if the server send a valid response enter */
			if(Local_u8RecStatus == 1)
			{
				/* if what inside flag.txt file = '1' enter */
				if(Local_u8flag == '1')
				{
					/* Connect again the microcontroller with the website */
					HESP_voidConnectOnServer("TCP" , "162.253.155.226" , "80");
					/* Get what inside temp.txt" file which contain a part of the hex file of the new application */
					Local_u8RecStatus = HESP_u8SendRequest("GET http://otaprog.freevar.com/TXT_Folder/temp.txt" , &u8RecBuffer , "52" , &u16RecLength , &u16RecIndex);
					/* if a valid data recieved enter */
					if (Local_u8RecStatus == 1)
					{
						/* u16RecIndex :is the first index of part of hex file data,
						 * u16RecLength :is the length of part of hex file data
						 */
						for(u16 i = u16RecIndex ; i< u16RecLength+u16RecIndex ; i++)
						{
							/* if flashing of data had finished and there is no data
							 * to get outside from the hex file, so enter the application
							 */
							if(u8RecBuffer[i] == 'N')
							{
								/* function that enter the application part in the
								 * flash memory
								 */
								func();
							}
							else
							{
								/* if the record finished and j is the number of the
								 * record indexes enter
								 */
								if(u8RecBuffer[i] == '\n' && j != 0)  /* This is beacause each hex file finish with empty line*/
								{
									/* check if the record is right or not passing the array
									 * of the record and the index number to a function
									 * to calculate the check sum
									 */
									u8CheckSum = Parser_u8CheckSum(u8FinalBuffer , j);
									/* if the record is right enter */
									if(u8CheckSum == 1)
									{
										/* u8BLWriteReq is initialized by 1, so if it is
										 * the first record it will enter the if condition
										*/
										if (u8BLWriteReq == 1)
										{
											/* Erase all the section of the application in
											 * the flash memory
											 */
											FPEC_voidEraseAppArea();
											/* make u8BLWriteReq = 0 to make the condition
											 * entered once
											 */
											u8BLWriteReq = 0;
										}
										/* Parase the record */
										Parser_voidParseRecord(u8FinalBuffer);
										/* clear the record index */
										j = 0;
									}
									else
									{
										break;
									}

								}
								else
								{
									/* fill the data inside the hex file record by record to
									 * a final buffer
									*/
									u8FinalBuffer[j] = u8RecBuffer[i];
									j++;
								}
							}
						}

					}
					else
					{

					}

					if(u8CheckSum == 1)
					{
						/* Connect the microcontroller to the server */
						HESP_voidConnectOnServer("TCP" , "162.253.155.226" , "80");
						/* Delete the value inside the flag.txt to indicate that the 30 records have been flashed */
						Local_u8RecStatus = HESP_u8SendRequest("POST /PHP_Folder/post.php HTTP/1.1\r\nHost: otaprog.freevar.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 7\r\n\r\nvalue=3\r\n" , &u8RecBuffer , "142" , &u16RecLength , &u16RecIndex);
					}
				}
			}
		}
   	 }
	else
	{
		func();
	}
}
