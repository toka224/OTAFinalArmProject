/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 12 AUG 2020                                */
/* Version   : V01                                        */
/**********************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "GPIO_interface.h"
#include "GPIO_private.h"
#include "GPIO_config.h"

#define GPIO_LCKK_PIN 16

/*************************************************************
 * [Func_Name]   : GPIO_voidSetPinDirection                  *
 * [Description] : Write MODE of the PINS in it's own port   *
                   register                                  *
 * [Args]        : copy_u8PORT,copy_u8Pin,copy_u8Mode        * 
 * [In]          : copy_u8PORT,copy_u8Pin,copy_u8Mode        *                       
 * [Out]         : NONE                                      *                       
 * [Return]      : NONE                                      *
 *************************************************************/
void GPIO_voidSetPinDirection(u8 copy_u8PORT , u8 copy_u8Pin , u8 copy_u8Mode)
{
	/* There are three ports (GPIOA,GPIOB,GPIOC) */
	
	/* GPIOA has pins from 0 --> 15
	 * GPIOB has pins from 0 --> 15
	 * GPIOC has 3 pins (C13,C14,C15)
	*/
	
	/* 
	 * copy_u8Mode has four bits : two bits for CNF 
	 * and another two for MODE.
	 * MODE : 00 --> input
			  01 --> output @10MHZ
			  00 --> output @2MHZ
			  00 --> output @50MHZ
	 * CNF  : input                           output
	          analog                00        push pull
			  floating		        01        open drain
			  pull up/pull down	    10        push pull(AF)
			  reversed     		    11        open drain(AF)
	 */
	switch(copy_u8PORT){
		
		/* CRL register has number of pins from 0 --> 7
		 * while CRH register has number of pins from 8 --> 15
		 */
		case GPIOA:
		
		      if(copy_u8Pin <= 7)
			  {
				  /* Clear the part of the register of 
				   * the required PIN 
				   */
				  GPIOA_CRL &= ~((0b1111) << (copy_u8Pin * 4));
				  /* write on the register the mode of the PIN */
				  GPIOA_CRL |= ((copy_u8Mode) << (copy_u8Pin * 4));
			  }
			  else if(copy_u8Pin <= 15)
			  {
				  /* From PIN 8 --> 15 it is included in another
				   * register, so we will subtract PIN number
				   * by 8 as the begaining of the register from 0	
				   */
				  copy_u8Pin = copy_u8Pin - 8;
				  /* Clear the part of the register of 
				   * the required PIN 
				   */
				  GPIOA_CRH &= ~((0b1111) << (copy_u8Pin * 4));
				  /* write on the register the mode of the PIN */
				  GPIOA_CRH |= ((copy_u8Mode) << (copy_u8Pin * 4));
			  }
			break;
	    case GPIOB:
				if(copy_u8Pin <= 7)
				{
				  GPIOB_CRL &= ~((0b1111) << (copy_u8Pin * 4));
				  GPIOB_CRL |=  ((copy_u8Mode) << (copy_u8Pin * 4));
				}
				else if(copy_u8Pin <= 15)
				{
				  copy_u8Pin = copy_u8Pin - 8;
				  GPIOB_CRH &= ~((0b1111) << (copy_u8Pin * 4));
				  GPIOB_CRH |= ((copy_u8Mode) << (copy_u8Pin * 4));
				}
			break;
		case GPIOC:
				if(copy_u8Pin <= 7)
				{
				  GPIOC_CRL &= ~((0b1111) << (copy_u8Pin * 4));
				  GPIOC_CRL |= ((copy_u8Mode) << (copy_u8Pin * 4));
				}
				else if(copy_u8Pin <= 15)
				{
				  copy_u8Pin = copy_u8Pin - 8;
				  GPIOC_CRH &= ~((0b1111) << (copy_u8Pin * 4));
				  GPIOC_CRH |= ((copy_u8Mode) << (copy_u8Pin * 4));
				}
			break;
	}
	
	
}

/*************************************************************
 * [Func_Name]   : GPIO_voidSetPinDirection                  *
 * [Description] : function used to set and reset pins using *
					ODR,BSRR,BRR registers                   *
 * [Args]        : copy_u8PORT,copy_u8PIN,copy_u8Value       * 
 * [In]          : copy_u8PORT,copy_u8PIN,copy_u8Value       *                       
 * [Out]         : NONE                                      *                       
 * [Return]      : NONE                                      *
 *************************************************************/
void GPIO_voidSetPinValue(u8 copy_u8PORT , u8 copy_u8PIN , u8 copy_u8Value)
{
	/* There are two methods to set DIO PIN 
	 * And three methods to reset PIN 
	 *(SET PIN) : ODR|=(1<<PIN)-->read ODR,change bit,write back to ODR
				  BSRR=(1<<PIN)-->write only
				  
	 *(RESET PIN) : ODR&=~(1<<PIN)-->read ODR,change bit,write back to ODR
				    BSRR=(1<<PIN+16)-->write only
					BRR=(1<<PIN)-->write only
	 
	 *BSRR is a register of 32 bits 16 for set and 16 for reset
	*/
	#if   SET_PIN_CHOICE == BSRR_BRR_BASED
	
				/* BRR & BSRR are used for the speed of the system */
		switch(copy_u8PORT){
		
			case GPIOA:
				if(copy_u8Value == GPIO_HIGH)
				{
		    	  /*set pin using BSRR register*/
		    	  GPIOA_BSR = (1 << copy_u8PIN );
				}
				else if(copy_u8Value == GPIO_LOW)
				{
				 /*clear pin using BRR register*/
					GPIOA_BRR = (1 << copy_u8PIN );
				}
				break;
			case GPIOB:
				if(copy_u8Value == GPIO_HIGH)
				{
				/*set pin using BSRR register*/
					GPIOB_BSR = (1 << copy_u8PIN);
				}
				else if(copy_u8Value == GPIO_LOW)
				{
					/*clear pin using BRR register*/
					GPIOB_BRR = (1 << copy_u8PIN );
				}
				break;
			case GPIOC:
				if(copy_u8Value == GPIO_HIGH)
				{
					/*set pin using BSRR register*/
					GPIOC_BSR = (1 << copy_u8PIN);
				}
				else if(copy_u8Value == GPIO_LOW)
				{

					/*clear pin using BRR register*/
					GPIOC_BRR = (1 << copy_u8PIN );
				}
				break;
	}
	#elif SET_PIN_CHOICE == ODR_BASED
		
		switch(Copy_u8Port)
		{
		case GPIOA:
			if( u8Copy_u8Value == HIGH ){

				SET_BIT( GPIOA_ODR , Copy_u8Pin );

			}else if( u8Copy_u8Value == LOW ){
				CLR_BIT(  GPIOA_ODR , Copy_u8Pin );
			}
			break;
		
		case GPIOB:
			if( u8Copy_u8Value == HIGH ){

				SET_BIT( GPIOB_ODR  , Copy_u8Pin );

			}else if( u8Copy_u8Value == LOW ){
				CLR_BIT( GPIOB_ODR  , Copy_u8Pin );
			}
			break;
		
		case GPIOC:
			if( u8Copy_u8Value == HIGH ){

				SET_BIT( GPIOC_ODR , Copy_u8Pin );

			}else if( u8Copy_u8Value == LOW ){
				CLR_BIT( GPIOC_ODR , Copy_u8Pin );
			}
			break;

	}
	#endif
}

/*************************************************************
 * [Func_Name]   : GPIO_u8GetPinValue                        *
 * [Description] : function to get value of a pin 0 or 1     *
 * [Args]        : copy_u8PORT,copy_u8Pin                    * 
 * [In]          : LOC_u8Result                              *                       
 * [Out]         : NONE                                      *                       
 * [Return]      : LOC_u8Result                              *
 *************************************************************/
u8 GPIO_u8GetPinValue(u8 copy_u8PORT , u8 copy_u8Pin)
{
	u8 LOC_u8Result = 0;
	switch(copy_u8PORT){
		
		case GPIOA:
		      
			LOC_u8Result = GET_BIT(GPIOA_IDR , copy_u8Pin );
			break;
	    case GPIOB:
				 
		    LOC_u8Result = GET_BIT(GPIOB_IDR , copy_u8Pin );
			break;
		case GPIOC:
		    LOC_u8Result = GET_BIT(GPIOC_IDR , copy_u8Pin );
			break;
	}
	
	return LOC_u8Result;
}

/*************************************************************
 * [Func_Name]   : GPIO_voidSetPortValue                     *
 * [Description] : write values on GPIO at a time            *
 * [Args]        : copy_u8PORT,copy_u16Value                 * 
 * [In]          : NONE                                      *                       
 * [Out]         : NONE                                      *                       
 * [Return]      : NONE                                      *
 *************************************************************/
void GPIO_voidSetPortValue(u8 copy_u8PORT , u16 copy_u16Value)
{
	/* Although BSRR & BRR are used for the speed 
	 * of the system,but ODR has the advantage of 
	 * writing values on all port at one time
	 */
	switch(copy_u8PORT)
	{

		/* read the value of ports from users 
		 * and write it on ODR registers
		*/
			case GPIOA: GPIOA_ODR = copy_u16Value;
				        break;
		    case GPIOB: GPIOB_ODR = copy_u16Value;
				        break;
			case GPIOC: GPIOC_ODR = copy_u16Value;
					    break;
	}
}

/*************************************************************
 * [Func_Name]   : GPIO_voidLockPin                          *
 * [Description] : Lock GPIO pins                            *
 * [Args]        : copy_u8Port,copy_u8Pin                    * 
 * [In]          : LOC_u8Reading1,LOC_u8Reading2             *                       
 * [Out]         : NONE                                      *                       
 * [Return]      : NONE                                      *
 *************************************************************/
void GPIO_voidLockPin(u8 copy_u8Port , u8 copy_u8Pin)
{
	u8 LOC_u8Reading1;
	u8 LOC_u8Reading2;
	switch(copy_u8Port)
	{
				/* set the required pin to be locked */
	case GPIOA:	SET_BIT(GPIOA_LCK , copy_u8Pin);
				/* set bit 16 in the lock register */
	            SET_BIT(GPIOA_LCK , GPIO_LCKK_PIN);
				/* clear bit 16 in the lock register */
	            CLR_BIT(GPIOA_LCK , GPIO_LCKK_PIN);
				/* set bit 16 in the lock register */
	            SET_BIT(GPIOA_LCK , GPIO_LCKK_PIN);
				/* get bit 16 in the lock register and put it in a variable */
	            LOC_u8Reading1 = GPIO_u8GetPinValue(GPIOA_LCK , GPIO_LCKK_PIN);
				/* get bit 16 in the lock register again and put it in a another variable */
	            LOC_u8Reading2 = GPIO_u8GetPinValue(GPIOA_LCK , GPIO_LCKK_PIN);
                if(!(LOC_u8Reading1 == 0 && LOC_u8Reading2 == 1))
                {
                    /*#error("Error while locking pin")*/
                }
		        break;
	case GPIOB: SET_BIT(GPIOB_LCK , copy_u8Pin);
	            SET_BIT(GPIOB_LCK , GPIO_LCKK_PIN);
				CLR_BIT(GPIOB_LCK , GPIO_LCKK_PIN);
				SET_BIT(GPIOB_LCK , GPIO_LCKK_PIN);
				LOC_u8Reading1 = GPIO_u8GetPinValue(GPIOB_LCK , GPIO_LCKK_PIN);
				LOC_u8Reading2 = GPIO_u8GetPinValue(GPIOB_LCK , GPIO_LCKK_PIN);
				if(!(LOC_u8Reading1 == 0 && LOC_u8Reading2 == 1))
				{
					/*#error("Error while locking pin")*/
				}
		        break;
	case GPIOC: SET_BIT(GPIOC_LCK , copy_u8Pin);
	            SET_BIT(GPIOC_LCK , copy_u8Pin);
				SET_BIT(GPIOC_LCK , GPIO_LCKK_PIN);
				CLR_BIT(GPIOC_LCK , GPIO_LCKK_PIN);
				SET_BIT(GPIOC_LCK , GPIO_LCKK_PIN);
				LOC_u8Reading1 = GPIO_u8GetPinValue(GPIOC_LCK , GPIO_LCKK_PIN);
				LOC_u8Reading2 = GPIO_u8GetPinValue(GPIOC_LCK , GPIO_LCKK_PIN);
				if(!(LOC_u8Reading1 == 0 && LOC_u8Reading2 == 1))
				{
					/*#error("Error while locking pin")*/
				}
				break;
	}

}

/****************************************************************
 * [Func_Name]   : GPIO_voidSetPortValueByRange                 *
 * [Description] : clear multiple pins at the same time         *
 * [Args]        : copy_u8PORT,copy_u16Value,copy_u8StartRange  *
                   ,copy_u8FinishRange                          * 
 * [In]          : NONE                                         *                       
 * [Out]         : NONE                                         *                       
 * [Return]      : NONE                                         *
 ****************************************************************/
void GPIO_voidSetPortValueByRange(u8 copy_u8PORT , u16 copy_u16Value , u8 copy_u8StartRange , u8 copy_u8FinishRange)
{
	switch(copy_u8PORT)
		{

			case GPIOA:
						/* looping throught the 16 pins of PORTA */
				        for(u8 i = 0 ; i < 16 ; i++)
				        {
							/* if the pin is at the required range enter*/
				        	if(i >= copy_u8StartRange && i <= copy_u8FinishRange)
				        	{
								/* clear the pin in the ODR register*/
				        		CLR_BIT(GPIOA_ODR , i);
				        	}
				        }
						/* */
				        GPIOA_ODR = GPIOA_ODR | (copy_u16Value << copy_u8StartRange);
						break;
			case GPIOB:
					   for(u8 i = 0 ; i < 16 ; i++)
						{
							if(i >= copy_u8StartRange && i <= copy_u8FinishRange)
							{
								CLR_BIT(GPIOB_ODR , i);
							}
						}
						GPIOB_ODR = GPIOB_ODR | (copy_u16Value << copy_u8StartRange);
						break;
			case GPIOC:
				       for(u8 i = 0 ; i < 16 ; i++)
						{
							if(i >= copy_u8StartRange && i <= copy_u8FinishRange)
							{
								CLR_BIT(GPIOC_ODR , i);
							}
						}
						GPIOC_ODR = GPIOC_ODR | (copy_u16Value << copy_u8StartRange);
						break;
		}
}
