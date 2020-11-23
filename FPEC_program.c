/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 12 oct 2020                                */
/* Version   : V01                                        */
/**********************************************************/

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "FPEC_interface.h"
#include "FPEC_private.h"
#include "FPEC_config.h"
/******************************************************************
 * [Func_Name]   : FPEC_voidEraseAppArea                          *
 * [Description] : Choose the pages in flash memory to be erased  *
 * [Args]        : NONE                                           * 
 * [In]          : i                                              *                        
 * [Out]         : i                                              *                       
 * [Return]      : NONE                                           *
 ******************************************************************/
void FPEC_voidEraseAppArea(void)
{
	u8 i;
	/* increment through the pages to be erased in the 
	 * the flash memory
	 */
	for (i=8;i<64;i++)
	{
		FPEC_voidFlashPageErase(i);
	}
}
/******************************************************************
 * [Func_Name]   : FPEC_voidFlashPageErase                        *
 * [Description] : - unlock FPEC to erase pages in flash memory   *
				   - Write the address of the page to be erased   *
 * [Args]        : Copy_u8PageNumber                              * 
 * [In]          : Copy_u8PageNumber                              *                        
 * [Out]         : NONE                                           *                       
 * [Return]      : NONE                                           *
 ******************************************************************/
void FPEC_voidFlashPageErase(u8 Copy_u8PageNumber)
{
	/* Wait Busy Flag which indicates that a Flash operation
     * is in progress
	 */
	while (GET_BIT(FPEC->SR,0) == 1);

	/* Check if FPEC is locked or not When Lock bit is set, 
	 * FPEC is locked. This bit is reset by hardware after
     * detecting the unlock sequence.
	 */
	if ( GET_BIT(FPEC->CR,7) == 1)
	{
		/* After reset, the FPEC block is protected.
		 * An unlocking sequence should be written to 
		 * the FLASH_KEYR register to open up the FPEC block.
		 */
		 
		    /* KEY1 */
		FPEC -> KEYR = 0x45670123;
			/* KEY2 */
		FPEC -> KEYR = 0xCDEF89AB;
	}
	
	/* Page Erase chosen Operation */
	SET_BIT(FPEC->CR,1);

	/* Write Page address */
	FPEC->AR = (u32)(Copy_u8PageNumber * 1024) + 0x08000000 ;

	/* Start operation 
	 * This bit triggers an ERASE operation when set.
     * This bit is set only by software and reset
	 * when the BSY bit is reset. 
	 */
	SET_BIT(FPEC->CR,6);

	/* Wait until erase operation finishes */
	while (GET_BIT(FPEC->SR,0) == 1);

	/* EOP Set by hardware when erase is completed.
     * So we need to reset it by writing 1 on it
	 */
	SET_BIT(FPEC->SR,5);
	
	/* Stop page Erase chosen Operation */
	CLR_BIT(FPEC->CR,1);
}

/********************************************************************
 * [Func_Name]   : FPEC_voidFlashWrite                              *
 * [Description] : - unlock FPEC to write on flash memory           *
				   - Write the data of each record on flash memory  *
 * [Args]        : Copy_u32Address,Copy_u16Data,Copy_u8Length       * 
 * [In]          : i , Temp                                         *                        
 * [Out]         : NONE                                             *                       
 * [Return]      : NONE                                             *
 ********************************************************************/
void FPEC_voidFlashWrite(u32 Copy_u32Address, u16* Copy_u16Data, u8 Copy_u8Length)
{
	u8 i;
	volatile u16 Temp;
	
	/* Wait Busy Flag which indicates that a Flash operation
     * is in progress
	 */
	while (GET_BIT(FPEC->SR,0) == 1);

	/* Check if FPEC is locked or not When Lock bit is set, 
	 * FPEC is locked
	 */
	if ( /* FPEC_CR->BitAccess.LOCK == 1 */ GET_BIT(FPEC->CR,7) == 1 )
	{
		
		    /* KEY1 */
		FPEC -> KEYR = 0x45670123;
		    /* KEY2 */
		FPEC -> KEYR = 0xCDEF89AB;
	}
	
	
	for (i = 0; i< Copy_u8Length; i++)
	{
		/* Write Flash Programming */
		SET_BIT(FPEC->CR,0);

		/* A temporary variable takes the value of each two 
		 * bytes of the data	
		 */
		Temp = Copy_u16Data[i];
		
		/* the flash driver write 2 bytes at a time until 
		 * the data in the record finishes 
		 */
		*((volatile u16*)Copy_u32Address) = Copy_u16Data[i];
		
		/* we add 2 because each time we store 2 bytes*/
		Copy_u32Address += 2 ; 

		/* Wait until writing on flash finishes or when 
         * an error occurs		
		 */
		while (GET_BIT(FPEC->SR,0) == 1);
	
		/* EOP Set by hardware when programming is completed.
		 * So we need to reset it by writing 1 on it
		 */
		SET_BIT(FPEC->SR,5);
		
		/* Stop write Flash Programming */
		CLR_BIT(FPEC->CR,0);
	}

}
