#include "delay.h"
#include "stm32F10x.h"

static double _new_time;		 //current updated time
static double _old_time;     //last updated time
static double _it_time;      //time = SysTick interrupt counter*1.8s

void sysclk_init()
{
  SysTick->CTRL &= 0xFFFFFFFB; //Clock div 8 = 9M  //   32 /8 = 4M
	SysTick->LOAD  = 16200000;   
	SysTick->CTRL |= 0x00000003; //INT +ENABLE
}

double GetNowTime()
{
	_new_time = _it_time + 1.8 - SysTick->VAL/9000000.0; //update current time
	
	if(_new_time - _old_time > 1.799) //check if breaked by SysTick interruptf
	{	
		_new_time -= 1.8;	              //calibrate current time
	}		          
	_old_time = _new_time;            //update old time
	return _new_time;
	
}


int ClockTool(double *record,double timeout)
{
	 double NowTime = GetNowTime();
		if(NowTime - *record >=timeout)
		{
			 *record = NowTime; //更新记录
			return 1;
		}
		else
			return 0;
}

void SysTick_Handler(void)
{
	 _it_time += 1.8;
}


//微秒延时
void DelayUs(u16 nus)
{
 	double OldT=GetNowTime();
	while((GetNowTime()-OldT)<nus/1000000.0);
}

//毫秒延时
void DelayMs(u16 nms)
{
	double OldT=GetNowTime();
	while((OldT=GetNowTime()-OldT)<(nms)/1000.0);
}

void DelayS(u16 ns)
{
}
