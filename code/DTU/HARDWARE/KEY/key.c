#include "key.h"
#include "sys.h" 
#include "delay.h"

#define  mode	0	//֧��������1����֧��������0
									  								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PD12,PD13,PD14,PD15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOE4

}

u8 KEY_Scan(void)
{	
	static u8 key_up=1;	//�������ɿ���־

	if(mode)
	{
		key_up=1;  		//֧������
	}
				  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);	//ȥ���� 
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

	return 0;	// �ް������� 
	
}

