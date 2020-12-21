#ifndef __USART_H
#define __USART_H
/*
USART1   TX:PA9  RX:PA10
USART2	 TX:PA2  RX:PA3
USART3	 TX:PB10  RX:PB11
*/
#include "stdio.h"	
#include "stm32f10x.h"

#define Use_Printf		1

		  
void usart_init(int buad);	 
void usart2_init(void);	  
void usart3_init(void);	  
void usart_send_char(USART_TypeDef* USARTx,char ch );  //发送一字节数据
void usart_send_string(USART_TypeDef* USARTx,char *ch);//发送字符串
void usart_send_buffer(uint8_t usart,uint8_t *data,int len);

void timer2_init(void);
void timer3_init(void);

void set_usart2_irq_callback( void (*callback)(void) );
void set_usart1_irq_callback( void (*callback)(void) );

int  GetSerialReciveDataSize(uint8_t usart);
void GetSerialReciveData(uint8_t usart,char *Data,int lenth);
void ClearSerialBuffer(uint8_t usart);

#endif

