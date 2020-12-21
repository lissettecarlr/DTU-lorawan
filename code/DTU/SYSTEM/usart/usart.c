#include "usart.h"	  
#include "fifo.h"


//改驱动将串口3作为打印输出
//串口1和定时器3绑定
//串口2和定时器2绑定


//串口中断收到的量					
static Fifo_t usart1_recive_fifo;
static char usart1_recive_buffer[255];

static Fifo_t usart2_recive_fifo;
static char usart2_recive_buffer[255];

static Fifo_t usart3_recive_fifo;
static char usart3_recive_buffer[255];


//get buffer lenth
int GetSerialReciveDataSize(uint8_t usart)
{ 
	 if(usart ==1 )
    return FifoDataLen(&usart1_recive_fifo);
	 else if(usart == 2)
	 {
	    return FifoDataLen(&usart2_recive_fifo);
	 }
	 else if(usart == 3)
	 {
	   return FifoDataLen(&usart3_recive_fifo);
	 }
	 else
		 return 0;
}

//传入一个buffer，读取长度
void GetSerialReciveData(uint8_t usart,char *Data,int lenth)
{
	 int i;
	 if(usart ==1 )
	 {
	   for(i=0;i<lenth;i++)
	    FifoPop( &usart1_recive_fifo, (Data+i) );
 	 } 
	 else if(usart == 2)
	 {
	   for(i=0;i<lenth;i++)
	    FifoPop( &usart2_recive_fifo, (Data+i) );
	 }
	 else if(usart == 3)
	 {
	   for(i=0;i<lenth;i++)
	     FifoPop( &usart3_recive_fifo, (Data+i) );
	 }
	 else
		 return;

}

void ClearSerialBuffer(uint8_t usart)
{
	if(usart ==1 )
      FifoFlush(&usart1_recive_fifo);
	 else if(usart == 2)
	 {
	    FifoFlush(&usart2_recive_fifo);
	 }
	 else if(usart == 3)
	 {
	    FifoFlush(&usart3_recive_fifo);
	 }
}


//加入以下代码,支持printf函数	  
#if Use_Printf
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 


//初始化IO 串口1 
void usart_init(int buad)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  									//复位串口1

	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); 					//初始化PA9
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  				//初始化PA10

   //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = buad;					//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); 		//初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启中断
    USART_Cmd(USART1, ENABLE);                    	//使能串口 
	 FifoInit( &usart1_recive_fifo, usart1_recive_buffer, 200 );
}


void usart2_init(void)
{
		
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//GPIOA时钟	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2,  ENABLE); //使能USART2
 	USART_DeInit(USART2);  									//复位串口2

	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 				//PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); 					//初始化PA9
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  				//初始化PA3

   //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;					//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); 		  //初始化串口
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启中断
  USART_Cmd(USART2, ENABLE);                    	//使能串口 
  FifoInit( &usart2_recive_fifo, usart2_recive_buffer, 200 );
}

void usart3_init(void)
{
	
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//GPIOA时钟
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3,  ENABLE);//使能USART3
 	USART_DeInit(USART3);  									//复位串口3

	 //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); 					//初始化PB10
   
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  				//初始化PB11

   //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = 115200;					//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); 		//初始化串口
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//开启中断
  USART_Cmd(USART3, ENABLE);                    	//使能串口 
  FifoInit( &usart3_recive_fifo, usart3_recive_buffer, 200 );
}


//发送一字节的数据 
void usart_send_char(USART_TypeDef* USARTx,char ch )
{ 
	USART_SendData( USARTx,(unsigned char) ch);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	USART_ClearFlag(USARTx, USART_FLAG_TXE);
} 

void usart_send_buffer(uint8_t usart,uint8_t *data,int len)
{
	  int i=0;
    switch(usart)
		{
			case 1:
				 USART_Cmd(USART1, ENABLE);
				 for(i=0;i<len;i++)
				 {
						USART_SendData( USART1,(unsigned char) data[i]);
						while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
						USART_ClearFlag(USART1, USART_FLAG_TXE);
				 }
				 //USART_Cmd(USART1, DISABLE);
				break;
			case 2:
				 USART_Cmd(USART2, ENABLE);
				 for(i=0;i<len;i++)
				 {
						USART_SendData( USART2,(unsigned char) data[i]);
						while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
						USART_ClearFlag(USART2, USART_FLAG_TXE);
				 }
				// USART_Cmd(USART2, DISABLE);
				break;
			case 3:
				 USART_Cmd(USART3, ENABLE);
				 for(i=0;i<len;i++)
				 {
						USART_SendData( USART3,(unsigned char) data[i]);
						while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
						USART_ClearFlag(USART3, USART_FLAG_TXE);
				 }
				 //USART_Cmd(USART3, DISABLE);
				break;
		}
}

//发送字符串
void usart_send_string(USART_TypeDef* USARTx,char *ch)
{
	while(*ch!='\0')
	{
		usart_send_char( USARTx,*ch );
		ch++;
	}
}


/***************************************TIMER**********************************************/
void timer2_init()
{
			TIM_OCInitTypeDef  TIM_OCInitStructure;
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
			NVIC_InitTypeDef NVIC_InitStructure;
			
		//  RCC_PCLK1Config(RCC_HCLK_Div1); 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

			//初始化定时器5 TIM5
			TIM_TimeBaseStructure.TIM_Period = 0xfffe; //设定计数器自动重装值 
			TIM_TimeBaseStructure.TIM_Prescaler =999; 	//预分频器   
			TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
		//  TIM_PrescalerConfig(TIM1, PrescalValue,TIM_PSCReloadMode_Immediate);
			//输出比较时间模式配置：通道1
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = 100;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			TIM_OC1Init(TIM2, &TIM_OCInitStructure);
			TIM_OC2Init(TIM2, &TIM_OCInitStructure);
			
			
			//TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);//禁止预装载使能
			//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//使能中断
			//TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);//使能中断
			//TIM_Cmd(TIM2,ENABLE ); 	//使能定时器 
			
			 TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);//清楚中断标识位
			 TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
			 
			 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
			 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级
			 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
			 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
			 NVIC_Init(&NVIC_InitStructure);
}

void timer3_init()
{
			TIM_OCInitTypeDef  TIM_OCInitStructure;
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
			NVIC_InitTypeDef NVIC_InitStructure;
			
		//  RCC_PCLK1Config(RCC_HCLK_Div1); 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

			TIM_TimeBaseStructure.TIM_Period = 0xfffe; //设定计数器自动重装值 
			TIM_TimeBaseStructure.TIM_Prescaler =999; 	//预分频器   
			TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
		//  TIM_PrescalerConfig(TIM1, PrescalValue,TIM_PSCReloadMode_Immediate);
			//输出比较时间模式配置：通道1
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = 100;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			TIM_OC1Init(TIM3, &TIM_OCInitStructure);
			
			//TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);//禁止预装载使能
			//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//使能中断
			//TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);//使能中断
			//TIM_Cmd(TIM2,ENABLE ); 	//使能定时器 
			
			 TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
			 TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);//清楚中断标识位
			 
			 NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
			 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级
			 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
			 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
			 NVIC_Init(&NVIC_InitStructure);
}

/***************************************TIMER**END*****************************************/


/***************************************FREE*USART*****************************************/
static uint8_t u1_Data_Start_Recive_Flag=0;//标志数据开始接收
static uint8_t u1_Data_recive_Flag=0;//标志数据正在接收

static uint8_t u2_Data_Start_Recive_Flag=0;//标志数据开始接收
static uint8_t u2_Data_recive_Flag=0;//标志数据正在接收

static uint8_t u3_Data_Start_Recive_Flag=0;//标志数据开始接收
static uint8_t u3_Data_recive_Flag=0;//标志数据正在接收

void usart_start_recive(uint8_t usart)
{
   switch (usart)
	 {
	   case 1:
				u1_Data_Start_Recive_Flag=0;
				u1_Data_recive_Flag=0;
				USART_Cmd(USART1, ENABLE);
		    ClearSerialBuffer(1);
		   break;
	   case 2:
				u2_Data_Start_Recive_Flag=0;
				u2_Data_recive_Flag=0;
				USART_Cmd(USART2, ENABLE);
		    ClearSerialBuffer(2);
		   break;
	   case 3:
				u3_Data_Start_Recive_Flag=0;
				u3_Data_recive_Flag=0;
				USART_Cmd(USART3, ENABLE);
		    ClearSerialBuffer(3);
		   break;		 
	 }
}

void usart_set_recive_start(uint8_t usart)
{ 
	  if(usart ==1)
		{
			FifoFlush(&usart1_recive_fifo);
			u1_Data_Start_Recive_Flag = 0;
			u1_Data_recive_Flag = 1;
				 
			TIM_Cmd(TIM3, ENABLE);//开启定时器
			TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
			TIM_SetCompare1(TIM3, TIM_GetCounter(TIM3)+2000); // 
		}
	  else if(usart == 2)
		{
			FifoFlush(&usart2_recive_fifo);
			u2_Data_Start_Recive_Flag = 0;
			u2_Data_recive_Flag = 1;
				 
			TIM_Cmd(TIM2, ENABLE);//开启定时器
			TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
			TIM_SetCompare1(TIM2, TIM_GetCounter(TIM2)+2000);
		}
		else if(usart ==3)
		{
		FifoFlush(&usart3_recive_fifo);
		u3_Data_Start_Recive_Flag = 0;
		u3_Data_recive_Flag = 1;
		   
	  TIM_Cmd(TIM3, ENABLE);//开启定时器
	  TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
		TIM_SetCompare1(TIM3, TIM_GetCounter(TIM3)+2000);

		}
}

void usart_set_recive_ing(uint8_t usart)
{
	if(usart ==1)
	{
		 u1_Data_Start_Recive_Flag = 0;
	   u1_Data_recive_Flag = 1;
	   TIM_SetCompare1(TIM3, TIM_GetCounter(TIM3)+2000);
	}
	else if(usart == 2)
	{
	  u2_Data_Start_Recive_Flag = 0;
	  u2_Data_recive_Flag = 1;
	  TIM_SetCompare1(TIM2, TIM_GetCounter(TIM2)+2000);
	}
  else if(usart ==3)
	{
		u3_Data_Start_Recive_Flag = 0;
	  u3_Data_recive_Flag = 1;
	  TIM_SetCompare1(TIM3, TIM_GetCounter(TIM3)+2000);
	}
}

void usart_set_recive_end(uint8_t usart)
{
	  if(usart ==1)
		{
			TIM_Cmd(TIM3, DISABLE);
			TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
			USART_Cmd(USART1, DISABLE);
			u1_Data_Start_Recive_Flag = 1;
			u1_Data_recive_Flag = 0;	
		}
	  else if(usart ==2)
		{
			TIM_Cmd(TIM2, DISABLE);
			TIM_ITConfig(TIM2,TIM_IT_CC1,DISABLE);
			USART_Cmd(USART2, DISABLE);
			u2_Data_Start_Recive_Flag = 1;
			u2_Data_recive_Flag = 0;	
		}
		else if(usart ==3)
		{
			TIM_Cmd(TIM3, DISABLE);
			TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
			USART_Cmd(USART3, DISABLE);
			u3_Data_Start_Recive_Flag = 1;
			u3_Data_recive_Flag = 0;	
		}
}

void usart_irq_free(uint8_t usart)
{
   switch(usart)
	 {
	   case 1:
				if(!u1_Data_Start_Recive_Flag && !u1_Data_recive_Flag) //需要获取数据且还未开始接收
				 {
						usart_set_recive_start(1);
				 }
				 else if(!u1_Data_Start_Recive_Flag && u1_Data_recive_Flag) //需要获取数据且正在接收
				 {
						usart_set_recive_ing(1);
				 }
			break;

	   case 2:
				if(!u2_Data_Start_Recive_Flag && !u2_Data_recive_Flag) //需要获取数据且还未开始接收
				 {
						usart_set_recive_start(2);
				 }
				 else if(u2_Data_Start_Recive_Flag && u2_Data_recive_Flag) //需要获取数据且正在接收
				 {
						usart_set_recive_ing(2);
				 }
			break;
				 
	   case 3:
				if(!u3_Data_Start_Recive_Flag && !u3_Data_recive_Flag) //需要获取数据且还未开始接收
				 {
						usart_set_recive_start(3);
				 }
				 else if(!u3_Data_Start_Recive_Flag && u3_Data_recive_Flag) //需要获取数据且正在接收
				 {
						usart_set_recive_ing(3);
				 }
			break;
	 }
}

/***************************************FIRE*USART**END************************************/

void (*usart2_irq_callback)(void);
void (*usart1_irq_callback)(void);

void set_usart2_irq_callback( void (*callback)(void) )
{
   usart2_irq_callback = callback;
}

void set_usart1_irq_callback( void (*callback)(void) )
{
   usart1_irq_callback = callback;
}

/***************************************IRQ***********************************************/

//串口1中断服务程序 
void USART1_IRQHandler(void)                	
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		usart_irq_free(1);
		if( FifoPush( &usart1_recive_fifo, (char)(USART_ReceiveData(USART1)) ) == 0 ) //存入数据
		{ 
		}
		else
		{ 
			//error
		} 	
  } 
}

//串口2中断服务程序
void USART2_IRQHandler(void)                
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{ 
		usart_irq_free(2);
		if( FifoPush( &usart2_recive_fifo, (char)(USART_ReceiveData(USART2)) ) == 0 ) //存入数据
		{
		}
		else
		{ 
			//error
		} 		 
  } 
} 

//串口3中断服务程序 该程序用于打印信息，不做存储处理
void USART3_IRQHandler(void)                
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		usart_irq_free(3);
//		if( FifoPush( &usart3_recive_fifo, (char)(USART_ReceiveData(USART3)) ) == 0 ) //存入数据
//		{}
//		else
//		{ 
//			//error
//		}
  } 
}


//定时器2中断服务程序
void TIM2_IRQHandler(void)  
{
	 if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		usart_set_recive_end(2);
		if(usart2_irq_callback !=NULL)
		{
			 usart2_irq_callback();
		}
		usart_start_recive(2);
	}
	if(TIM_GetITStatus(TIM2, TIM_FLAG_Update) != RESET)
	{
	  	 TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	}
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
		 TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);  //清除TIMx更新中断标志
     usart_set_recive_end(1);
		 if(usart1_irq_callback !=NULL)
		 {
			 usart1_irq_callback();
		 }
		 usart_start_recive(1);
	}
	if(TIM_GetITStatus(TIM3, TIM_FLAG_Update) != RESET)
  {
	 TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
	 //Timer3_Update_IRQ();  
  }
}

/***************************************IRQ**END******************************************/


/***************************************CALLBACK******************************************/



/***************************************CALLBACK**END*************************************/


