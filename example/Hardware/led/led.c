#include "Led.h"

void Led_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 						
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	delay_ms(10);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}

void Led_On(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

void Led_Off(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}

void Led_Times(int ms,int times)
{
	int i=0;
	for(i=0;i<times;i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_15);
		delay_ms(ms);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		if(i+1!=times)delay_ms(ms);
	}
	
}	
