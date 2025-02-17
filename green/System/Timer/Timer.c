#include "Timer.h"

//TIM2/3

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1;	//分辨率1us,最大60ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	
	
}

void Timer_delay_us(int xus)
{
	TIM_Cmd(TIM2,ENABLE); //启动定时器
	while(TIM2->CNT < xus);
	TIM2->CNT = 0;
	TIM_Cmd(TIM2,DISABLE); //关闭定时器
}

void Timer_delay_ms(int xms)
{
	int i=0;
	for(i=0;i<xms;i++)Timer_delay_us(1000);
}

//外部中断专用延时函数
void EXIT_LINE_Timer_delay_us(int xus)
{
	TIM_Cmd(TIM3,ENABLE); //启动定时器
	while(TIM3->CNT < xus);
	TIM3->CNT = 0;
	TIM_Cmd(TIM3,DISABLE); //关闭定时器
}

void EXIT_LINE_Timer_delay_ms(int xms)
{
	int i=0;
	for(i=0;i<xms;i++)EXIT_LINE_Timer_delay_us(1000);
}
