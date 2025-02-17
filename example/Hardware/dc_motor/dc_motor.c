#include "dc_motor.h"

int Duty=0;				//PWM占空比
int Direction=0;		//小车行驶方向（急刹时用）
float Speed_Cm_S=0;		//小车速度cm/s
//extern int order;
int Sum_Pulse=0;		//行驶产生的总脉冲数

void Dc_Motor_Stop(void)
{
	In1_Off;
	In2_Off;
	Direction=0;
	Dc_Motor_SetDuty(0);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);			//还原正常向上计数模式
//	In3_Off;
//	In4_Off;
}

//设置小车行驶方向为前进
void Dc_Motor_Go(void)
{
	In1_On;
	In2_Off;
	Direction=1;
//	In3_On;
//	In4_Off;
}

//设置小车行驶方向为后退
void Dc_Motor_Back(void)
{
	In1_Off;
	In2_On;
	Direction=-1;
//	In3_Off;
//	In4_On;
}

//void Dc_Motor_Right(void)
//{
//	In1_On;
//	In2_Off;
//	In3_Off;
//	In4_Off;
//}

//void Dc_Motor_Left(void)
//{
//	In1_Off;
//	In2_Off;
//	In3_On;
//	In4_Off;
//}

//小车急停（反向制动）
void Dc_Motor_Emerg_Stop(void)
{
	//根据行驶速度简单设定刹车方向（反向制动）
	if(Direction>0)Dc_Motor_Back();
	else if(Direction<0)Dc_Motor_Go();

	//根据目前速度设定急刹占空比和时间
	Dc_Motor_SetDuty(5*Speed_Cm_S);
	delay_ms(4+((int)Speed_Cm_S/15)%5);
	Dc_Motor_Stop();
}

//设置控制小车速度的PWM波的占空比
void Dc_Motor_SetDuty(int duty)
{
	int temp=duty;
	if(temp<0)temp=0;
	if(temp>500)temp=500;				//限幅处理
	TIM_SetCompare2(TIM3,temp);
	TIM_SetCompare1(TIM3,temp);
}

//通过PID设置小车行驶速度
//设置行驶速度的PID参数
float DC_Motor_Speed_Kp=1;
float DC_Motor_Speed_Ki=0.1;
float DC_Motor_Speed_Kd=5;		//D值大防止过冲
void Dc_Motor_SetSpeed_PID(float Target_Speed_Cm_S)
{
	int time_out=0;
	int right_times=0;
	float target_speed=Target_Speed_Cm_S;
	
	//控制小车行驶方向
	if(Target_Speed_Cm_S>0){
		Dc_Motor_Go();
		TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);		//正转编码器向上计数
	}
	else if(Target_Speed_Cm_S<0)
	{
		Dc_Motor_Back();
		target_speed=-Target_Speed_Cm_S;
		TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);		//反转编码器向上计数
	}
	
	
	int duty=0;
	while(1)
	{
		Led_Times(4,1);
		time_out++;
		
		duty=Duty+Pid_Control(DC_Motor_Speed_Kp,DC_Motor_Speed_Ki,DC_Motor_Speed_Kd,target_speed,Speed_Cm_S,PID_INCREASE);
		Dc_Motor_SetDuty(duty);
		
		if(Speed_Cm_S>target_speed-1&&Speed_Cm_S<target_speed+1)		//判断是否达到设定速度
		{
			right_times++;
			if(right_times>=10)break;									//判断速度是否稳定
		}
		else right_times=0;
		
		if(time_out>=100)
		{
			BlueTooth_TransmitPrintf("TIME OUT\r\n");
			Beep_Times(3,2);
			break;														//超时直接退出，防止功率不足卡死
		}
	}
	Beep_Times(3,1);
}

//简单算法设置小车行驶速度
void Dc_Motor_SetSpeed(float Target_Speed_Cm_S)
{
	int time_out=0;
	int right_times=0;
	float target_speed=Target_Speed_Cm_S;
	
	//控制小车行驶方向
	if(Target_Speed_Cm_S>0){
		Dc_Motor_Go();
		TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);		//正转编码器向上计数
	}
	else if(Target_Speed_Cm_S<0)
	{
		Dc_Motor_Back();
		target_speed=-Target_Speed_Cm_S;
		TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);		//反转编码器向上计数
	}
	
	while(1)
	{
		Led_Times(4,1);
		time_out++;
		
		if(Speed_Cm_S<Target_Speed_Cm_S-4||Speed_Cm_S>Target_Speed_Cm_S+4)Dc_Motor_SetDuty(Duty+(Target_Speed_Cm_S-Speed_Cm_S)/4);
		else{
			if(Speed_Cm_S<Target_Speed_Cm_S-1)Dc_Motor_SetDuty(Duty+1);
			else if(Speed_Cm_S>Target_Speed_Cm_S+1)Dc_Motor_SetDuty(Duty-1);
		}
		
		//判断是否达到设定速度
		if(Speed_Cm_S>target_speed-1&&Speed_Cm_S<target_speed+1)		
		{
			right_times++;
			if(right_times>=10)break;									//判断速度是否稳定
		}
		else right_times=0;
		
		if(time_out>=100)
		{
			BlueTooth_TransmitPrintf("TIME OUT\r\n");
			Beep_Times(3,2);
			break;														//超时直接退出，防止功率不足卡死
		}
	}
	Beep_Times(3,1);
}

//小车行驶定长距离，支持负距离（后退）；Gate为考虑负载的占空比阈值
//设置行驶距离的PID参数
float DC_Motor_Length_Kp=3.5;
float DC_Motor_Length_Ki=10;		//I值大防止负载导致行驶过慢
float DC_Motor_Length_Kd=5;		//D值大防止过冲
void Dc_Motor_Move(float Target_Length_Cm,int Gate)
{
	//运行前打开Speed_Get()（已在0.1s定时中断中打开）
	
	int target_length=Target_Length_Cm;
	int duty=0;
	float Sum_Length=0;
	//设定行驶方向
	if(Target_Length_Cm<0){
		Dc_Motor_Back();
		target_length=-Target_Length_Cm;
		TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);		//设置反转编码器依旧向上计数
	}
	else Dc_Motor_Go();
	
	//计算行驶距离并调整速度
	Sum_Pulse=0;
	Sum_Length=0;
	while(Sum_Length < target_length-2)
	{
		Sum_Length=Sum_Pulse*PER_PULSE/10;
		duty=Pid_Control(DC_Motor_Length_Kp,DC_Motor_Length_Ki,DC_Motor_Length_Kd,target_length,Sum_Length, PID_INCREASE);
		Dc_Motor_SetDuty(duty+Gate);	//考虑负载，移动阈值为20
	}
	
	//考虑惯性，急停
	Dc_Motor_Emerg_Stop();
}

//直流电机初始化
void Dc_Motor_Init(void)
{
	//注意！！！这PB3/4/PA15三个引脚默认的是JLink的复用功能，如果程序中还有其他GPIO口的配置，那这三个引脚的GPIO初始化一定要放在其他所有GPIO配置之后，否则依然无法作为普通IO使用。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 
	
	//Disable JLink, enable SW
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    

   	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	Dc_Motor_Stop();
}

//显示直流电机目前状况（占空比）
void Show_DC_Motor_State()
{	
	//显示L298N的IN1234口电平情况（测试用）
//	OLED_ShowString(1,1,"order:");
//	OLED_ShowNum(1,7,order,1);
//	OLED_ShowNum(2,1,GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_13),1);
//	OLED_ShowNum(2,3,GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_12),1);
//	OLED_ShowNum(3,1,GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9),1);
//	OLED_ShowNum(3,3,GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8),1);	
	Speed_Cm_S=Speed_Get(0.1);
	OLED_ShowString(1,1,"Speed:   cm/s");
	OLED_ShowNum(1,7,Speed_Cm_S,3);
	
	Duty=TIM_GetCapture1(TIM3);
	OLED_ShowString(2,1,"Duty:   %");
	OLED_ShowNum(2,7,Duty,3);
}

//外部时钟计数器编码器模式（接直流电机编码器）初始化
void Encoder_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;							//配置GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;					//上拉输入 GPIO_Mode_IN_FLOATING浮空输入（编码器输出波形峰值0.5v左右）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//默认内部时钟，可省略
	TIM_InternalClockConfig(TIM4);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//配置时基单元
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;				//ARR	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;				//PSC
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	//编码器接口配置（通路见PPT）
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);	

	TIM_Cmd(TIM4, ENABLE);
	
	Dc_Motor_SetDuty(0);
	
}

//获取编码器计数值（直流减速电机正转转动产生脉冲数）
int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp = TIM_GetCounter(TIM4);
	TIM_SetCounter(TIM4, 0);
	return Temp;
}

//返回小车速度（cm/s）
float Speed_Get(int Time_S)
{
	int Pulse_Num=Encoder_Get();
	//如果后退则用最大值减去当前值
	Sum_Pulse+=Pulse_Num;				//总脉冲数累加（Dc_Motor_Move()用到）
	float Speed=Pulse_Num*0.134;		//实为Pulse_Num*PER_PULSE/Time_S/10，但这样写会出错？
	return Speed;
}

//简单遥控功能实现
void Dc_Motor_Mode(){
	int duty;					//占空比设定值
	int angle=0;
	int Length=0;
	float speed=0;
	int order=0;				//蓝牙接收到的命令
	int sign=1;					//循环标志位
	
	while(sign){
		//蓝牙接收信号处理部分
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
			BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
			BlueTooth_TransmitPrintf("\r\nOK\r\n");
			order=BlueTooth_RxPack[0];
		}
		switch(order){
			case 0:Dc_Motor_Stop();break;
			case 1:Dc_Motor_Go();break;
			case 2:Dc_Motor_Back();break;
			case 3:duty=duty+10;Dc_Motor_SetDuty(duty);break;
			case 4:duty=duty-10;Dc_Motor_SetDuty(duty);break;
			case 5:Length+=5;Dc_Motor_Move(Length,0);break;
			case 6:Length-=5;Dc_Motor_Move(Length,0);break;
			case 7:angle+=10;Set_Angle(angle);break;
			case 8:angle-=10;Set_Angle(angle);break;
			case 9: speed+=10;Dc_Motor_SetSpeed_PID(speed);break;
			case 10:speed-=10;Dc_Motor_SetSpeed_PID(speed);break;
			case 11:Turn_Left_90();break;
			case 12:Turn_Right_90();break;
			case 13:Fast_Turn_Left_90();break;
			case 14:Fast_Turn_Right_90();break;
			case 15:sign=0;break;	//退出循环
		}
		order=-1;
	}
}

void Simple_Aviod_Obstacle(){
	//简单避障功能实现
	
	extern float distance;		//超声波测距值					//cs100a.c
	int Simple_Aviod_Obstacle_Stop_Sign=1;
	while(Simple_Aviod_Obstacle_Stop_Sign)
	{
		if(distance<=400){
			Dc_Motor_SetDuty(-100);Set_Angle(-60);
		}
		else if(distance>500&&distance<=1000){
			Dc_Motor_SetDuty(100);Set_Angle(60);
		}
		else{
			Dc_Motor_SetDuty(300);Set_Angle(0);
		}
	}
}
