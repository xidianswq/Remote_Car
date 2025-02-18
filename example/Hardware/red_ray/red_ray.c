#include "red_ray.h"

void Red_Ray_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 						
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	delay_ms(10);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
}

void Red_Ray_Off(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
}

void Red_Ray_On(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}
