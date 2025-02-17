#include "public.h"
#include "bluetooth.h"
#include "oled.h"
#include "Pwm_1.h"

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
	int i=0;
	
	//主程序
	while(1)
	{
		//
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
			BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
			BlueTooth_TransmitPrintf("\r\nOK\r\n");
		}
		
		switch(order){
			case 0:break;
			case 1:break;
			case 2:break;
			case 3:break;
			case 4:break;
			case 5:break;
			case 6:break;
			case 7:break;
			case 8:break;
			case 9:break;	
			case 10:break;
			case 11:break;
			case 12:break;
			case 13:break;
			case 14:break;
			case 15:break;
		
		//测试部分
		i=(i+1)%20;
		OLED_ShowNum(2,1,i,3);
		TIM_SetCompare1(TIM1,175+i);
	}
}
