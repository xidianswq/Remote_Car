#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"                  // Device header

//使用的延时函数类型
#define NORMAL_MODE 0
#define EXIT_LINE_MODE 1

void Timer_Init(void);
void Timer_delay_us(int xus);
void Timer_delay_ms(int xms);
void EXIT_LINE_Timer_delay_us(int xus);
void EXIT_LINE_Timer_delay_ms(int xms);

#endif
