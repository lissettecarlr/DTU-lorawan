#include "key.h"
#include "sys.h" 
#include "delay.h"

#define  mode	0	//支持连按置1，不支持连按置0
									  								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PD12,PD13,PD14,PD15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE4

}

u8 KEY_Scan(void)
{	
	static u8 key_up=1;	//按键按松开标志

	if(mode)
	{
		key_up=1;  		//支持连按
	}
				  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);	//去抖动 
		key_up=0;

		if(KEY1==0)
			return 1;
		else if(KEY2==0)
			return 2;
		else if(KEY3==0)
			return 3;
		else if(KEY4==0)
			return 4;
	}
	else if(KEY1&&KEY2&&KEY3&&KEY4)
	{
		key_up=1; 
	}		    

	return 0;	// 无按键按下 
	
}

