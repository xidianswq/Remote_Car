#include "public.h"
#include "bluetooth.h"
#include "oled.h"
#include "Pwm_1.h"
#include "dc_motor.h"

#include "string.h"		

int main(void)
{
	//初始化程序
	//测试工具初始化
	BlueToothInit(9600,USART_Parity_No,USART_StopBits_1,USART_WordLength_8b);
	OLED_Init();
	OLED_ShowString(1,1,"hello!");
	
	//被测试部分初始化
	TIM1_PWM_Init(199,7199);
	Dc_Motor_Init();
	int order=0;
	OLED_ShowString(2,1,"order:");
	
	//主程序
	while(1)
	{
		//
		In3_Off;
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
			BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
			BlueTooth_TransmitPrintf("\r\nOK\r\n");
			order=BlueTooth_RxPack[0];
		}
		
		//测试部分
		//TIM_SetCompare1(TIM1,180);
		switch(order){
			case 0:Dc_Motor_Stop();break;
			case 1:Dc_Motor_Go();break;
			case 2:Dc_Motor_Back();break;
			case 3:Dc_Motor_Left();break;
			case 4:Dc_Motor_Right();break;
		}
		OLED_ShowNum(2,7,order,1);
		OLED_ShowNum(3,1,GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_12),1);
		OLED_ShowNum(3,3,GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_11),1);
		OLED_ShowNum(4,1,GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_10),1);
		OLED_ShowNum(4,3,GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_9),1);
	}
}
