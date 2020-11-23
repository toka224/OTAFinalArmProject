/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 12 oct 2020                                */
/* Version   : V01                                        */
/**********************************************************/

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "FPEC_interface.h"
#include "PARSE_interface.h"
#include "PARSE_config.h"
#include "PARSE_private.h"

/* initiate array of hex value each location has two bytes
 * of data to be send to FPEC be written by the flash driver
 */
u16 Data[100] = {0};

/* initiate the address of the record that represent the address 
 * of the application part inside the flash memory
 */
u32 Address= 0x08000000;
/***********************************************************
 * [Func_Name]   : AsciToHex                               *
 * [Description] : Enter the Ascii value and convert it to *
                   hexadecimal value.                      *
 * [Args]        : Copy_u8Asci                             * 
 * [In]          : Copy_u8Asci,Result                      *                        
 * [Out]         : Result                                  *                       
 * [Return]      : Result                                  *
 ***********************************************************/

u8 AsciToHex(u8 Copy_u8Asci)
{
	u8 Result;
	
	if ( (Copy_u8Asci >= 48) && (Copy_u8Asci <= 57) )
	{
	   /* Ascii of numbers from 0 to 9 
		* Equal from 48 to 57
		* So we subtract 48 to be converted to it's hex value
		*/
		Result = Copy_u8Asci - 48;
	}
	
	else
	{
	   /* Ascii of numbers from A to F 
		* Equal from 65 to 70
		* In Hexadecimal from (10 to 15) equivalent to (A to F)
		* So we subtract 55 to be converted to it's hex value
		*/
		Result = Copy_u8Asci - 55;
	}
	/* returns the hexadecimal value */
	return Result;
}

/**************************************************************
 * [Func_Name]   : ParseData                                  *
 * [Description] : - Read information inside the Hex file.    *
				   - Read values of (Character count,Adress   *
					 ,Data).                                  *
				   - Merge each info in a variable or array   *
				     of variables.                            *
				   - Send the read info to FPEC Write         *
				     function.	                              *
 * [Args]        : Copy_u8BufData                             * 
 * [In]          : Copy_u8BufData,DigitLow,DigitHigh,CC,i     *
				   DataDigit0,DataDigit1,DataDigit2,          *
				   DataDigit3,DataCounter                     * 
 * [Out]         : Address,Data,CC/2                          *                       *
 * [Return]      : None                                       *
 **************************************************************/

void ParseData(u8* Copy_u8BufData)
{
	u8 DigitLow,DigitHigh,CC,i;
	u8 DataDigit0,DataDigit1,DataDigit2,DataDigit3;
	u8 DataCounter = 0;

	/* Get Character Count which is the first one byte  
	 * after (:) character in each record
	 */
	DigitHigh = AsciToHex (Copy_u8BufData[1]);//The First half byte in CC
	DigitLow  = AsciToHex (Copy_u8BufData[2]);//The Second half byte in CC
	/* Merge the two half bytes in one byte */
	CC        = (DigitHigh<<4) | DigitLow ;

	/* Get Address which are the first two bytes after CC in each record*/
	DataDigit0 = AsciToHex (Copy_u8BufData[3]);//The First half byte in the address
	DataDigit1 = AsciToHex (Copy_u8BufData[4]);//The Second half byte in the address
	DataDigit2 = AsciToHex (Copy_u8BufData[5]);//The Third half byte in the address
	DataDigit3 = AsciToHex (Copy_u8BufData[6]);//The Fourth half byte in the address

	/* Clear Low Part of Address */
	Address = Address & 0xFFFF0000;
	
	/* Merge the four half bytes in two bytes */
	Address = Address | (DataDigit3) | (DataDigit2 << 4) | (DataDigit1 << 8) | (DataDigit0<<12);

	/* Get Data that will be written in the flash memomy   
	 * by the flash driver which is from bit 9 in 
	 * the hex file 
	 */
	for (i=0;i<CC/2; i++)
	{
		DataDigit0 = AsciToHex (Copy_u8BufData[4*i+9 ]);
		DataDigit1 = AsciToHex (Copy_u8BufData[4*i+10]);
		DataDigit2 = AsciToHex (Copy_u8BufData[4*i+11]);
		DataDigit3 = AsciToHex (Copy_u8BufData[4*i+12]);

		/* Merge each four half bytes of data in two bytes 
		 * to have an array of two bytes in each location 
		 */
	    Data[DataCounter] = (DataDigit3 << 8) | (DataDigit2 << 12) | (DataDigit1) | (DataDigit0<<4);   /* Shouldn`t be Digit0 << 12 , Digit1 << 8 , Digit2 << 4 , Digit3 not shifted??j*/
    		
		DataCounter++;
	}
	
	/* Call the write function in FPEC */
	FPEC_voidFlashWrite(Address,Data,CC/2);
}

void ParseUpperAddress(u8* Copy_u8BufData)
{
//	u8 DataDigit0,DataDigit1,DataDigit2,DataDigit3;
//
//	/* Get Address */
//	DataDigit0 = AsciToHex (Copy_u8BufData[3]);
//	DataDigit1 = AsciToHex (Copy_u8BufData[4]);
//	DataDigit2 = AsciToHex (Copy_u8BufData[5]);
//	DataDigit3 = AsciToHex (Copy_u8BufData[6]);
//
//	/* Clear Low Part of Address */
//	Address = Address & (u32)0x0000FFFF;
//	Address = Address | (u32)((u32)(DataDigit3 << 28) | (u32)(DataDigit2 << 24) | (u32)(DataDigit1 << 20) | (u32)(DataDigit0 << 16));
}

/***********************************************************
 * [Func_Name]   : Parser_voidParseRecord                  *
 * [Description] : Indicate if the record holds data or    *
                   extended address                        *
 * [Args]        : Copy_u8BufData                          * 
 * [In]          : Copy_u8BufData                          * 
 * [Out]         : Copy_u8BufData                          *
 * [Return]      : None                                    *
 ***********************************************************/
void Parser_voidParseRecord(u8* Copy_u8BufData)
{
	/* Read the second half byte value of RT and the first  
	 * half byte always equal to zero
	 * Because if RT = 00 --> data record
	 * Else if RT = 04 --> extended address 
	 */
	switch (Copy_u8BufData[8])
	{
	case '0': ParseData(Copy_u8BufData); break;
	
	/* When RT = 04 the data after RT is equal to
	 * the the first two bytes of the addresse of 
	 * the flash memory that the data of the application 
	 * will be written in it
	 */
	case '4': ParseUpperAddress(Copy_u8BufData); break;
	}
}

/***********************************************************
 * [Func_Name]   : Parser_u8CheckSum                       *
 * [Description] : Fuction to calculate check sum of each  *
                   record and compare it with the last     *
                   byte inside the record				   *
 * [Args]        : Copy_u8BufData,Copy_u8RecordLength      * 
 * [In]          : Copy_u8BufData,Copy_u8RecordLength,i    * 
                   ,Local_u8Sum,Digit1,Digit2,Data         * 
 * [Out]         : None                                    *
 * [Return]      : 1 or 0                                  *
 ***********************************************************/
u8 Parser_u8CheckSum(u8* Copy_u8BufData , u8 Copy_u8RecordLength)
{
	u8 i , Local_u8Sum = 0 ;
	volatile u8 Digit1 , Digit2 , Data;
	
	/* This loop reads info inside the Hex file from the 
	 * second location of the array of the record 
	 * Because (Copy_u8BufData[0] = :) is not a data
	 * till the end of the record except the last two 
	 * locations which represent the check sum
	 */
	for(i = 1 ; i <= (Copy_u8RecordLength - 2)/2 ; i++)
	{
		Digit1 = AsciToHex(Copy_u8BufData[2*i-1]);
		Digit2 = AsciToHex(Copy_u8BufData[2*i]);
		/* Merge the two half bytes into one byte */
		Data   = (Digit1 << 4) | Digit2;
		/* Add each byte of the record to a variable except
		 * the check sum value
		 */
        Local_u8Sum += Data;
	}

	Local_u8Sum = 1 + ~(Local_u8Sum);
	
	/* The First half byte of the check sum */
	Digit1 = AsciToHex(Copy_u8BufData[Copy_u8RecordLength-2]);
	
	/* The Second half byte of the check sum */
	Digit2 = AsciToHex(Copy_u8BufData[Copy_u8RecordLength-1]);
	
	/* Merge the two half bytes into one byte */
	Data   = (Digit1 << 4) | Digit2;

	/* Compare the calculated check sum with the check sum
     * which is obtained inside the record
	 */
	if(Data == Local_u8Sum)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
