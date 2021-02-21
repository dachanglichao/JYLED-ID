#ifndef __USART_H
#define __USART_H

#define DK_USART1 1
#define DK_USART2 2
#define DK_USART3 3

extern void DK_USART_Init(unsigned long Baud,unsigned char USARTx);
extern void DK_USART_Send_Char(unsigned char USARTx,unsigned char data);
extern unsigned char DK_USART_WAIT_RXBYTE(unsigned char USARTx);
extern void DK_USART_PUTS(unsigned char USARTx,unsigned char *buffer);
extern void DK_USART_PUTS_Const(unsigned char USARTx,const unsigned char *buffer);
extern void DK_USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len);
extern void DK_USART1_Printf(unsigned char *p,unsigned char *dat);
extern void DK_USART_OPen_INT(unsigned char USARTx);
extern void DK_USART_Close_INT(unsigned char USARTx);
extern void	Init_Serial(void);
extern void	SerialSendArray (unsigned char *data,unsigned char len);
extern void	SerialSendChar (unsigned char aa);
extern void PsamUSART_Configuration(void);

#endif
