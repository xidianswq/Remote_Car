#include "dc_motor.h"

void Dc_Motor_Stop(void)
{
	In1_Off;
	In2_Off;
	In3_Off;
	In4_Off;
}

void Dc_Motor_Go(void)
{
	In1_On;
	In2_Off;
	In3_On;
	In4_Off;
}

void Dc_Motor_Back(void)
{
	In1_Off;
	In2_On;
	In3_Off;
	In4_On;
}

void Dc_Motor_Left(void)
{
	In1_On;
	In2_Off;
	In3_Off;
	In4_Off;
}

void Dc_Motor_Right(void)
{
	In1_Off;
	In2_Off;
	In3_On;
	In4_Off;
}

void Dc_Motor_Init(void)
{
	void Stop(void);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	Dc_Motor_Stop();
}
