#ifndef __SYSTICK_H__
#define __SYSTICK_H__
#include "stm32f10x.h"

void sysclk_init(void);
double GetNowTime(void);
int ClockTool(double *record,double timeout);
void DelayMs(uint16_t nms);
#endif
