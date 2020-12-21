#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
 	 
//#define KEY0 PEin(4)   //PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  
 
#define KEY1  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)//��ȡ����2
#define KEY3  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)//��ȡ����3 
#define KEY4  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)//��ȡ����4 


void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(void);  	//����ɨ�躯��
					    
#endif
