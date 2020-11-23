/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 28 SEP 2020                                */
/* Version   : V02                                        */
/**********************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "GPIO_interface.h"
#include "UART_interface.h"
#include "UART_config.h"
#include "UART_private.h"

/********************************************************
 * [Func_Name]   : MUART1_voidInit                      *
 * [Description] : - Write baud rate value              *
				   - Set TX,RX and UART                 *
 * [Args]        : NONE                                 * 
 * [In]          : NONE                                 *                        
 * [Out]         : NONE                                 *                       
 * [Return]      : NONE                                 *
 ********************************************************/
void MUART1_voidInit(void)
{
	/* BRR register recieves the value of baud rate.
	 * The baud counters stop counting 
	 * if the TE or RE bits are disabled
	 */
	/* 9600     BRR = 0x341 */
	/* 115200   BRR = 0x45 */
	/* baud rate div = Fclk/(16*baud rate) = 8000000/(16*115200) = 3.34
	 * mantissa = 4 , fraction = 0.34
	 * 0.34*16 = 5.44 = 5
	 * mantissa = 4 , fraction = 5
	 * so BRR=0x45
	*/
	/* Set baudrate */
	MUART1->BRR = 0x45;
	
	/* Enable RX */
	SET_BIT(MUART1->CR1 , 2);
	/* Enable TX */
    SET_BIT(MUART1->CR1 , 3);
	/* Enable UART */
	SET_BIT(MUART1->CR1 , 13);
	
	/* Clear Status Register */
	MUART1->SR = 0;

}

/**********************************************************
 * [Func_Name]   : MUART1_voidTransmit                    *
 * [Description] : Trasmit the data that microcontroller  *
				   will send to others                    *
 * [Args]        : arr[]                                  * 
 * [In]          : arr[],i                                *                        
 * [Out]         : NONE                                   *                       
 * [Return]      : NONE                                   *
 **********************************************************/
void MUART1_voidTransmit(u8 arr[])
{
	/* The function will recieve the data from the user 
	 * in an array and it will put it inside the data 
	 * register location by location until '\0' so that
	 * the data has been finished
	 */
	u8 i = 0;
	while(arr[i] != '\0')
	{
		MUART1->DR = arr[i];
		
		/* Wait till transmission complete*/
		while(GET_BIT( MUART1->SR , 6) == 0);
		i++;
	}
}

/********************************************************************
 * [Func_Name]   : MUART1_u8Receive                                 *
 * [Description] : Recieve data from others or recieve value        *
				   indicates that there a time out to recieve data  *
 * [Args]        : Copy_u32timeout                                  * 
 * [In]          : Copy_u32timeout,timeout,Local_u8ReceivedData     *                        
 * [Out]         : Local_u8ReceivedData                             *                       
 * [Return]      : Local_u8ReceivedData                             *
 ********************************************************************/
u8   MUART1_u8Receive(u32 Copy_u32timeout)
{
	
	u32 timeout = 0;
	u8 Local_u8ReceivedData = 0;
	/* Enter the while loop if there no data recieved */
	while(GET_BIT(MUART1->SR , 5) == 0)
	{
		/* Inside the loop timeout variable increments 
		 * until it has the same value as the argument 
		 * or there is a data recieved so go outside 
		 * the loop
		 */
		timeout++;
		/* if timeout has completed */
		if(timeout == Copy_u32timeout)
		{
			/* return number to indicate that there 
			 * time out happened
			 */
			Local_u8ReceivedData = 255;
			break;
		}
	}
	/* if before the timeout is completed there is 
	 * a data recieved
	 */
	if(Local_u8ReceivedData == 0)
	{
		/* recieve the data from the register in a variable*/
		Local_u8ReceivedData =  ((MUART1->DR) & 0xFF);
	}

	return Local_u8ReceivedData;
}
