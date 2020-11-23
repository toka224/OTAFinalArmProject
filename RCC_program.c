/*********************************************************************************/
/* Author    : Toka Medhat                                                       */
/* Version   : V01                                                               */
/* Date      : 8 August 2020                                                     */
/*********************************************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "RCC_interface.h"
#include "RCC_private.h"
#include "RCC_config.h"

/********************************************************
 * [Func_Name]   : RCC_voidInitSysClock                 *
 * [Description] : Initialize RCC clock                 *
 * [Args]        : NONE                                 * 
 * [In]          : NONE                                 *                        
 * [Out]         : NONE                                 *                       
 * [Return]      : NONE                                 *
 ********************************************************/
void RCC_voidInitSysClock(void)
{
	
	#if     RCC_CLOCK_TYPE == RCC_HSE_CRYSTAL
		/* Enable HSE with no bypass */
		RCC_CR   = 0x00010000; 
		/* HSE selected as system clock */
		RCC_CFGR = 0x00000001;
		
	#elif   RCC_CLOCK_TYPE == RCC_HSE_RC
		/* Enable HSE with bypass */
		RCC_CR   = 0x00050000; 
		/* HSE selected as system clock */
		RCC_CFGR = 0x00000001;
		
	#elif   RCC_CLOCK_TYPE == RCC_HSI
		/* The default value of HSITRIM is (16)--> (10000) */
		/* Enable HSI with the default Trimming value */
		RCC_CR   = 0x00000081; 
		RCC_CFGR = 0x00000000;
	
	#elif   RCC_CLOCK_TYPE == RCC_PLL
		#if   RCC_PLL_INPUT == RCC_PLL_IN_HSI_DIV_2
		
		#elif RCC_PLL_INPUT == RCC_PLL_IN_HSE_DIV_2
	
		#elif RCC_PLL_INPUT == RCC_PLL_IN_HSE
		
		#endif
	
	#else
		#error("You chosed Wrong Clock type")
	#endif
}


/********************************************************
 * [Func_Name]   : RCC_voidEnableClock                  *
 * [Description] : Enable the RCC of the peripherals    *
 * [Args]        : Copy_u8BusId,Copy_u8PerId            * 
 * [In]          : NONE                                 *                        
 * [Out]         : NONE                                 *                       
 * [Return]      : NONE                                 *
 ********************************************************/
void RCC_voidEnableClock(u8 Copy_u8BusId, u8 Copy_u8PerId)
{
	/* AMBA is a collection of protocalls (AHB,APB).
	 * There are APB1 and APB2.
	 * All peripherals which are connected to the ARM
	 * are connected to AMBA.
	 */
	/* There are three registers for the three buses
	 * and each register has 32 bits from 0 --> 31,
	 * so normally the ID the peripheral not to exceed 31.
	 * if the user enter a valid ID enter the condition.
	 */
	if (Copy_u8PerId <= 31)
	{
		switch (Copy_u8BusId)
		{
			/* Enable RCC of the peripheral by setting it's bit in it's 
			 * own register
			 */
			case RCC_AHB  : SET_BIT(RCC_AHBENR  ,Copy_u8PerId);   break;
			case RCC_APB1 : SET_BIT(RCC_APB1ENR ,Copy_u8PerId);   break;
			case RCC_APB2 : SET_BIT(RCC_APB2ENR ,Copy_u8PerId);   break;
		}
	}
	
	else
	{
		/* Return Error */
	}

}

/********************************************************
 * [Func_Name]   : RCC_voidDisableClock                 *
 * [Description] : Disable the RCC of the peripherals   *
 * [Args]        : Copy_u8BusId,Copy_u8PerId            * 
 * [In]          : NONE                                 *                        
 * [Out]         : NONE                                 *                       
 * [Return]      : NONE                                 *
 ********************************************************/
void RCC_voidDisableClock(u8 Copy_u8BusId, u8 Copy_u8PerId)
{
	if (Copy_u8PerId <= 31)
	{
		switch (Copy_u8BusId)
		{
			/* Disable RCC of the peripheral by clearing it's bit in it's 
			 * own register
			 */
			case RCC_AHB  : CLR_BIT(RCC_AHBENR  ,Copy_u8PerId);   break;
			case RCC_APB1 : CLR_BIT(RCC_APB1ENR ,Copy_u8PerId);   break;
			case RCC_APB2 : CLR_BIT(RCC_APB2ENR ,Copy_u8PerId);   break;
		}
	}
	
	else
	{
		/* Return Error */
	}

}

