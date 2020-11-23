/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 28 SEP 2020                                */
/* Version   : V01                                        */
/**********************************************************/

#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "UART_interface.h"
#include "ESP_interface.h"
#include "ESP_config.h"
#include "ESP_private.h"

u8 *Global_u8ReceivePointer = NULL;// Global pointer contains all message recieved from UART
u8 Global_u8Flag  = 0;
u16 *Global_u16Length = NULL;
u16 *Global_u16Index    = NULL;
/********************************************************************
 * [Func_Name]   : FHESP_voidInit                                   *
 * [Description] : - send initial cammands to stop Echo and to set  *
				     station mode                                   *
                   - wait until the righ message to be recieved	    *
 * [Args]        : NONE                                             * 
 * [In]          : Local_u8Result,Resp[]                            *                        
 * [Out]         : NONE                                             *                       
 * [Return]      : NONE                                             *
 ********************************************************************/
void HESP_voidInit(void)
{
	u8 Local_u8Result = 0;
	   Global_u8Flag  = 1;
    static u8 Resp[50] ={0};
    Global_u8ReceivePointer = Resp;
	/* Inside the while loop the first message to 
	 * stop echo it will wait until it will recieve
	 * return 1 which means that there is appreviation 
	 * message recieved
	 */
	while(Local_u8Result == 0)
	{
		/* Stop Echo command*/
		/* (\r\n) = ENTER + Ctrl +j */
	    MUART1_voidTransmit("ATE0\r\n");
		/* There are two values that Local_u8Result 
		 * 1 or 0. 1 means that there is an appreviation
		 * message was recieved, 0 means that no message 
		 * or no appreviation message was recieved, so
		 * it will transmit the command once again
		 */
	    Local_u8Result = voidESPValidateCmd(500); //100  //500
	}
	
	Local_u8Result = 0;
	while(Local_u8Result == 0)
	{
		/* Set station mode command */
		/*(\r\n):(\r) means ENTER button in the keyboard,
		 *(\n) means Ctrl + j buttons in the keyboard
		 */
		MUART1_voidTransmit("AT+CWMODE=1\r\n");
		/* Local_u8Result = 1 the message recieved 
		 * in a righ way so exit from the loop.
		 * Local_u8Result = 0 the message recieved 
		 * in a wrong way or not recieved anything
		 * so you have to transmit the command again
		 */
	    Local_u8Result = voidESPValidateCmd(500); //150  //500
	}
}

/********************************************************************
 * [Func_Name]   : HESP_voidConnectOnWiFi                           *
 * [Description] : - send a cammand to be connected with WIFI       *
				   - wait until the righ message to be recieved     *
 * [Args]        : Copy_u8UserName,Copy_u8Password                  * 
 * [In]          : Local_u8Result,Resp[]                            *                        
 * [Out]         : NONE                                             *                       
 * [Return]      : NONE                                             *
 ********************************************************************/
void HESP_voidConnectOnWiFi(u8* Copy_u8UserName , u8* Copy_u8Password)
{
	u8 Local_u8Result = 0;
       Global_u8Flag  = 1;
       static u8 Resp[50] ={0};
       Global_u8ReceivePointer = Resp;

	/* while loop to approve that the message is transmited 
	 * in a righ way and also to be recieved an appreviation
	 * command
	 */
	while(Local_u8Result == 0)
	{
	   /* the message is transmited through UART to 
	    * make the microcontroller to be connected with
		* the WIFI 
	    */
	   /*   AT+CWJAP_CUR="wifiname","passwordofwifi"   */
	   MUART1_voidTransmit("AT+CWJAP_CUR=\"");
	   MUART1_voidTransmit(Copy_u8UserName);
	   MUART1_voidTransmit("\",\"");
	   MUART1_voidTransmit(Copy_u8Password);
	   /* (\r\n) = ENTER + Ctrl +j */
	   MUART1_voidTransmit("\"\r\n");
	   /* wait for 1100000 counts or less till you get the 
	    * appreviation message else send the command again
		*/
	   Local_u8Result = voidESPValidateCmd(1100000);   //1100000
	}

}

/********************************************************************
 * [Func_Name]   : HESP_voidConnectOnServer                         *
 * [Description] : - send a cammand to be connected with the server *
				   - wait until the righ message to be recieved     *
 * [Args]        : Copy_u8Mode,Copy_u8IP,Copy_u8Port                * 
 * [In]          : Local_u8Result,Resp[]                            *                        
 * [Out]         : NONE                                             *                       
 * [Return]      : NONE                                             *
 ********************************************************************/
void HESP_voidConnectOnServer(u8* Copy_u8Mode , u8* Copy_u8IP , u8* Copy_u8Port)
{
	u8 Local_u8Result = 0;
	   Global_u8Flag  = 1;
	   static u8 Resp[50] ={0};
	     Global_u8ReceivePointer = Resp;

	while(Local_u8Result == 0)
	{
	   /* the message is transmited through UART to 
	    * make the microcontroller to be connected with
		* the server (website)
	    */
	   /*   AT+CIPSTART="TCP","IP",PORT   */
	   MUART1_voidTransmit("AT+CIPSTART=\"");
	   MUART1_voidTransmit(Copy_u8Mode);
	   MUART1_voidTransmit("\",\"");
	   MUART1_voidTransmit(Copy_u8IP);
	   MUART1_voidTransmit("\",");
	   MUART1_voidTransmit(Copy_u8Port);
	   MUART1_voidTransmit(",");
	   MUART1_voidTransmit(10);
	   /* (\r\n) = ENTER + Ctrl +j */
	   MUART1_voidTransmit("\r\n");
	   /* wait for 300000 counts or less till you get the 
	    * appreviation message else send the command again
		*/
	   Local_u8Result = voidESPValidateCmd(300000);   //300000

	}
}

/**********************************************************************
 * [Func_Name]   : HESP_u8SendRequest                                 *
 * [Description] : - send a cammand contains the lenght of the data   *
                     command and wait until the righ message to be    *
					 recieved                                         *
				   - send a data cammand and wait until the hex file  *
              		 data to be recived		                          *
 * [Args]        : Copy_u8Data,Copy_u8ReceiveData,Copy_u8Length       *   
				   ,Copy_u16RecLength,Copy_u16RecIndex                * 
 * [In]          : Local_u8Result                                     *                        
 * [Out]         : NONE                                               *                       
 * [Return]      : Local_u8Result                                     *
 **********************************************************************/
u8 HESP_u8SendRequest(u8* Copy_u8Data , u8* Copy_u8ReceiveData , u8* Copy_u8Length , u16* Copy_u16RecLength, u16* Copy_u16RecIndex)
{
	volatile u8 Local_u8Result = 0;
    Global_u8ReceivePointer = Copy_u8ReceiveData;
    Global_u16Length = Copy_u16RecLength;
    Global_u16Index = Copy_u16RecIndex;
	while(Local_u8Result == 0)
	{
	   MUART1_voidTransmit("AT+CIPSEND=");
	   /* Copy_u8Length is equal to the lengh of the
		* Copy_u8Data that the user introduces.
		*/
	   MUART1_voidTransmit(Copy_u8Length);
	   /* (\r\n) = ENTER + Ctrl +j */
	   MUART1_voidTransmit("\r\n");
	   /* make Global_u8Flag = 1 to have an authority
		* to get a certain kind of recieved message
		*/
	   Global_u8Flag  = 1;
	   /* wait for 50000 counts or less till you get the 
	    * appreviation message else send the command again
		*/
       Local_u8Result = voidESPValidateCmd(50000);    //300   //50000  //500
	}

	Local_u8Result = 0;

	while(Local_u8Result == 0)
	{
	   MUART1_voidTransmit(Copy_u8Data);
	   /* (\r\n) = ENTER + Ctrl +j */
	   MUART1_voidTransmit("\r\n");
	   /* Global_u8Flag  = 0 here after this command because 
	    * i dont want at this time to get an appreviation message 
		* i want to get data  which is the hex file to flash it
		* in the application section in the flash memory
		*/
	   Global_u8Flag  = 0;
	   /* wait for 900000 counts or less till you get the data 
	    * (length + hex file data) else send the data command again
		*/
	   Local_u8Result = voidESPValidateCmd(900000);  //3000000  //900000
	}
	return Local_u8Result;
}

/***************************************************************************
 * [Func_Name]   : voidESPValidateCmd                                      *
 * [Description] : - Show an approve if the message is valid or not        *
				   - calculate the length of the data of the hex file      *
				   - save the index that the hex file data will start from *
 * [Args]        : Copy_u32timeout                                         * 
 * [In]          : i,Dummy,Result                                          *                        
 * [Out]         : NONE                                                    *                       
 * [Return]      : Result                                                  *
 ***************************************************************************/
static u8 voidESPValidateCmd(u32 Copy_u32timeout)
{
	
    volatile u16 i = 0;
	u8 Dummy = 0 ;
	u8 Result = 0;
	volatile u16 Local_u16Count = 0;
	/* Dummy variable will get the value of 255 when time is out 
	 * which means UART hasnot recieved any thing so at the first 
	 * time it will enter the loop as Dummy = 0 and inside the loop
	 * it will take the value of 255 or appreviation message or 
	 * error or data message which includes the hex file data 
	 */
	while(Dummy != 255)
	{
		/* Wait for a certain time until data to be recieved */
		Dummy =  MUART1_u8Receive(Copy_u32timeout);
		/* put all messages in an array */
		Global_u8ReceivePointer[i] = Dummy;
		/* increment until the data message finishes */
		i++;
		/* here the last recieved message is 255, so go outside the loop */
	}
	i = 0;
	/* if the array includes only 255 which means no message recieved 
	 * not to enter the while loop and return the value of 0 as the initial 
	 * value of the Result is 0
	 */
	while(Global_u8ReceivePointer[i] != 255)
	{
		
		/* (,) this character is the character before hex file length
		 * so if the array includes this char enter the condition
		 */
		if(Global_u8ReceivePointer[i] == ',')
		{
			/* The second increment must be all or a part of the hex file 
			 * length 
			 */
			i++;
			/* (:) this character is the character before hex file data
			 * so if the array of index i still not include this char
			 * enter the condition
			 */
			while(Global_u8ReceivePointer[i] != ':')
			{
				/* This loop is to calculate the length of the hex file 
				 * and it will be recieved ascii so firstly we will convert 
				 * it to hex
				 */
				
				Local_u16Count = (Local_u16Count * 10) +(Global_u8ReceivePointer[i] - 48);
				/* index will be incremented until the value of the length to be calculated*/
				i++;
			}
			/* After the loop Global_u8ReceivePointer[i] = ':'  */
			/* we will increment the index to have the first value of the hex file data*/
			*Global_u16Index= i+1;
			/* Length of the data inside the hex file*/
            *Global_u16Length = Local_u16Count;
			/* Result = 1 to indicate that the data had arrived successfully*/
			Result = 1;
			break;
		}
	    /* Check if Ok */
		else if((Global_u8ReceivePointer[i] == 'O' && Global_u8ReceivePointer[i+1] == 'K') && Global_u8Flag == 1)
		{
			Result = 1;
			break;
		}
		/* Check if ALR */
		else if(((Global_u8ReceivePointer[i]== 'A' && Global_u8ReceivePointer[i+1] == 'L') && Global_u8ReceivePointer[i+2] == 'R') && Global_u8Flag == 1)
		{
			Result = 1;
			break;
		}
		/* Check if CONNEC */
		else if((Global_u8ReceivePointer[i]== 'C' && Global_u8ReceivePointer[i+1] == 'O' && Global_u8ReceivePointer[i+2] == 'N' && Global_u8ReceivePointer[i+3]== 'N' && Global_u8ReceivePointer[i+4] == 'E' && Global_u8ReceivePointer[i+2] == 'C') && Global_u8Flag == 1)
		{
			Result = 1;
			break;
		}
		
       i++;
	}
	return Result;
}


