#include "stm32f10x.h"
#include "led.h"


void LED_SET_ON()
{

	  GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

void LED_SET_OFF()
{
     GPIO_SetBits(GPIOB,GPIO_Pin_5);	
}

void LED_REVERSE()
{
  static uint8_t flag=0;
	if(flag == 0 )
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		flag =1;
	}
	else
	{
	  flag =0;
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
	}
}
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 				
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 					
}
 
