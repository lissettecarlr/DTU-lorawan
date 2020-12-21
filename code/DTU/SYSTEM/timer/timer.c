//#include "timer.h"



////extern char SEND_FLAG;	
//////arr：自动重装值。
//////psc：时钟预分频数
//////这里使用的是定时器3!
////void TIMx_Int_Init(u16 arr,u16 psc)
////{
////  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
////	NVIC_InitTypeDef NVIC_InitStructure;

////	#if timer3
////	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
////	
////	//定时器TIM3初始化
////	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
////	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
////	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
////	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
////	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
//// 
////	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

////	//中断优先级NVIC设置
////	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
////	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

////	TIM_Cmd(TIM3, ENABLE);  //使能TIMx	
////	#endif

////	#if timer4
////	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
////	
////	//定时器TIM3初始化
////	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
////	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
////	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
////	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
////	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
//// 
////	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

////	//中断优先级NVIC设置
////	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  //从优先级3级
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
////	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

////	TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
////	#endif

////	#if timer5
////	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
////	
////	//定时器TIM3初始化
////	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
////	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
////	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
////	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
////	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
//// 
////	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

////	//中断优先级NVIC设置
////	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;  //从优先级3级
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
////	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

////	TIM_Cmd(TIM5, ENABLE);  //使能TIMx	
////	#endif


////					 
////}


////定时器2中断服务程序
//void TIM2_IRQHandler(void)   //TIM3中断
//{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
//		
//	}
//}

////定时器3中断服务程序
//void TIM3_IRQHandler(void)   //TIM3中断
//{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
//	{
//		 TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志
//     	
//	}
//}

//////定时器4中断服务程序
////void TIM4_IRQHandler(void)   //TIM3中断
////{
////	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
////	{
////		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
////	}
////}


//void timer2_init()
//{
//			TIM_OCInitTypeDef  TIM_OCInitStructure;
//			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//			NVIC_InitTypeDef NVIC_InitStructure;
//			
//		//  RCC_PCLK1Config(RCC_HCLK_Div1); 
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//			//初始化定时器5 TIM5
//			TIM_TimeBaseStructure.TIM_Period = 0xfffe; //设定计数器自动重装值 
//			TIM_TimeBaseStructure.TIM_Prescaler =999; 	//预分频器   
//			TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//		//  TIM_PrescalerConfig(TIM1, PrescalValue,TIM_PSCReloadMode_Immediate);
//			//输出比较时间模式配置：通道1
//			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
//			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//			TIM_OCInitStructure.TIM_Pulse = 100;
//			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//			TIM_OC1Init(TIM2, &TIM_OCInitStructure);
//			TIM_OC2Init(TIM2, &TIM_OCInitStructure);
//			
//			
//			//TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);//禁止预装载使能
//			//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//使能中断
//			//TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);//使能中断
//			//TIM_Cmd(TIM2,ENABLE ); 	//使能定时器 
//			
//			 TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);//清楚中断标识位
//			 TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
//			 
//			 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//			 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级
//			 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
//			 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//			 NVIC_Init(&NVIC_InitStructure);
//}

//void timer3_init()
//{
//			TIM_OCInitTypeDef  TIM_OCInitStructure;
//			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//			NVIC_InitTypeDef NVIC_InitStructure;
//			
//		//  RCC_PCLK1Config(RCC_HCLK_Div1); 
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

//			TIM_TimeBaseStructure.TIM_Period = 0xfffe; //设定计数器自动重装值 
//			TIM_TimeBaseStructure.TIM_Prescaler =999; 	//预分频器   
//			TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//		//  TIM_PrescalerConfig(TIM1, PrescalValue,TIM_PSCReloadMode_Immediate);
//			//输出比较时间模式配置：通道1
//			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
//			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//			TIM_OCInitStructure.TIM_Pulse = 100;
//			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//			TIM_OC1Init(TIM3, &TIM_OCInitStructure);
//			
//			//TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);//禁止预装载使能
//			//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//使能中断
//			//TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);//使能中断
//			//TIM_Cmd(TIM2,ENABLE ); 	//使能定时器 
//			
//			 TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);//清楚中断标识位
//			 TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
//			 
//			 NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//			 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级
//			 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
//			 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//			 NVIC_Init(&NVIC_InitStructure);
//}

