#include "stm32f10x.h"  // Device header
#include "servo_motor.h"
#include "Pwm_1.h"
#include "bluetooth.h"

int main(void)
{
	BlueToothInit(9600,USART_Parity_No,USART_StopBits_1,USART_WordLength_8b);	//蓝牙初始化
	TIM1_PWM_Init(9999,143);	//一周期20ms，分辨率20ms/10000）
	int order;
	int angle_l=0;
	int angle_v=0;
	while(1)
	{
		//蓝牙接收信号处理部分
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
		BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
		BlueTooth_TransmitPrintf("\r\nOK\r\n");
		order=BlueTooth_RxPack[0];
		}

		//蓝牙控制直流电机工作
		//TIM_SetCompare1(TIM1,180);
		switch(order){
			case 0:angle_l=0;Set_Angle_Level(angle_l);break;	
			case 1:angle_l+=10;Set_Angle_Level(angle_l);break;
			case 2:angle_l-=10;Set_Angle_Level(angle_l);break;
			case 3:angle_v=0;	Set_Angle_Vert(angle_v);break;	
			case 4:angle_v+=10;Set_Angle_Vert(angle_v);break;
			case 5:angle_v-=10;Set_Angle_Vert(angle_v);break;
		}	
		order=-1;
	}
}
