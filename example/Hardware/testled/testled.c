#include"testled.h"
void TestledInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef TESTLED_GPIO_InitStructure;
	TESTLED_GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	TESTLED_GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	TESTLED_GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&TESTLED_GPIO_InitStructure);
}

