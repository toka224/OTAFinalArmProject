/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 28 SEP 2020                                */
/* Version   : V02                                        */
/**********************************************************/
#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

void MUART1_voidInit(void);
void MUART1_voidTransmit(u8 arr[]);
u8   MUART1_u8Receive(Copy_u32timeout);

#endif
