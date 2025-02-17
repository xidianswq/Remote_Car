#include "ray.h"
#define VAILD 0
int L_2=0;
int L_1=0;
int C=0;
int R_1=0;
int R_2=0;
int State=0;	//小车运行状态：0为停止，1为直行

//初始化读取红外传感器的GPIO
void Ray_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//读取各个红外传感器数值
void Get_Ray_State(void)
{
	L_2=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
	L_1=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
	C=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
	R_1=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
	R_2=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
}

//红外传感状态机模版，外部状态变化更改Stop_Sign以退出循环
void Ray_State_Action(void)
{
	int Stop_Sign=1;//各子功能停止位
	while(Stop_Sign)
	{
		Get_Ray_State();
		//全未检测到直线
		if(L_2==L_1&&L_1==C&&C==R_1&&R_1==R_2&&R_2==!VAILD)
		{
			;
		}
		//只最中间的检测到直线
		else if(L_2==L_1&&L_1==R_1&&R_1==R_2&&R_2==!VAILD&&C==VAILD)
		{
			;
		}
		//只左一检测到直线
		else if(L_2==C&&C==R_1&&R_1==R_2&&R_2==!VAILD&&L_1==VAILD)
		{
			;
		}
		//只左二检测到直线
		else if(L_1==C&&C==R_1&&R_1==R_2&&R_2==!VAILD&&L_2==VAILD)
		{
			;
		}
		//只右一检测到直线
		else if(R_2==C&&C==L_1&&L_1==L_2&&L_2==!VAILD&&R_1==VAILD)
		{
			;
		}
		//只右二检测到直线
		else if(R_1==C&&C==L_1&&L_1==L_2&&L_2==!VAILD&&R_2==VAILD)
		{
			;
			
		}
		//全检测到直线
		else if(L_2==L_1&&L_1==C&&C==R_1&&R_1==R_2&&R_2==VAILD)
		{
			;
		}
		//其他情况
		else
		{
			;
		}
	}
}

//巡线程序，Duty介于[0,500]
void Follow_Track(int duty)
{
	int Follow_Track_Stop_Sign=1;
	while(Follow_Track_Stop_Sign)
	{
		Get_Ray_State();
		//全未检测到直线
		if(L_2==L_1&&L_1==C&&C==R_1&&R_1==R_2&&R_2==!VAILD)
		{
//			Dc_Motor_Stop();
		}
		//只最中间的检测到直线
		else if(L_2==L_1&&L_1==R_1&&R_1==R_2&&R_2==!VAILD&&C==VAILD)
		{
			//直行
			Set_Angle(0);	
			Dc_Motor_Go();
			Dc_Motor_SetDuty(duty);
		}
		//只左一检测到直线
		else if(L_2==C&&C==R_1&&R_1==R_2&&R_2==!VAILD&&L_1==VAILD)
		{
			//轻微左转
			Set_Angle(-30);
			Dc_Motor_Go();
			Dc_Motor_SetDuty(duty);
		}
		//只左二检测到直线
		else if(L_1==C&&C==R_1&&R_1==R_2&&R_2==!VAILD&&L_2==VAILD)
		{
			//左转
			Set_Angle(-60);
			Dc_Motor_Go();
			Dc_Motor_SetDuty(duty);
		}
		//只右一检测到直线
		else if(R_2==C&&C==L_1&&L_1==L_2&&L_2==!VAILD&&R_1==VAILD)
		{
			//轻微右转
			Set_Angle(30);
			Dc_Motor_Go();
			Dc_Motor_SetDuty(duty);
		}
		//只右二检测到直线
		else if(R_1==C&&C==L_1&&L_1==L_2&&L_2==!VAILD&&R_2==VAILD)
		{
			//右转
			Set_Angle(60);
			Dc_Motor_Go();
			Dc_Motor_SetDuty(duty);
		}
		//全检测到直线
		else if(L_2==L_1&&L_1==C&&C==R_1&&R_1==R_2&&R_2==VAILD)
		{
//			Dc_Motor_Stop();
		}
		//其他情况
		else
		{
			;
		}
	}
}
