/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 28 SEP 2020                                */
/* Version   : V01                                        */
/**********************************************************/

#ifndef ESP_INTERFACE_H
#define ESP_INTERFACE_H

void HESP_voidInit(void);
void HESP_voidConnectOnWiFi(u8* Copy_u8UserName , u8* Copy_u8Password);
void HESP_voidConnectOnServer(u8* Copy_u8Mode , u8* Copy_u8IP , u8* Copy_u8Port);
u8 HESP_u8SendRequest(u8* Copy_u8Data , u8* Copy_u8Receive , u8* Copy_u8Length , u16* Copy_u16RecLength , u16* Copy_u16RecIndex);
u8 HESP_u8GetStatus(u8* Copy_u8Status);
#endif
