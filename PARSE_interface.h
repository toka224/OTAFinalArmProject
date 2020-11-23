/**********************************************************/
/* Author    : Toka Medhat                                */
/* Date      : 12 oct 2020                                */
/* Version   : V01                                        */
/**********************************************************/

#ifndef PARSE_INTERFACE
#define PARSE_INTERFACE

void Parser_voidParseRecord(u8* Copy_u8BufData);
u8 Parser_u8CheckSum(u8* Copy_u8BufData , u8 Copy_u8RecordLength);

#endif
