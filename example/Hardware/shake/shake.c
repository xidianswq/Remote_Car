#include "shake.h"
#include "bluetooth.h"
void ShakeInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef SHAKE_GPIO_InitStructure;
	SHAKE_GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	SHAKE_GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	SHAKE_GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&SHAKE_GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init (&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

void EXTI4_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line4)==SET)
	{
		/*
		判断陀螺仪角度，若倾倒，执行报警
		判断是否解锁，若为解锁，说明被人移动，鸣响一下，若持续移动，报警
		*/
		BlueTooth_TransmitPrintf("ATTENTION!\r\n");
		;
		EXTI_ClearFlag(EXTI_Line4);
	}
}
