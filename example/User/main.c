//*********************************************头文件声明*********************************************
#include "public.h"
#include "bluetooth.h"
#include "oled.h"
#include "Pwm_1.h"
#include "Pwm_2.h"
#include "dc_motor.h"
#include "servo_motor.h"
#include "AD.h"
#include "string.h"	
#include "Delay.h"
#include "Timer.h"
#include "cs100a.h"
#include "usart.h"
#include "MPU6050.h"
#include "beep.h"
#include "led.h"
#include "ray.h"
#include "pca9685.h"
#include "dma.h"

//********************************************引脚使用说明********************************************
/*
oled.h				GPIOA PIN0/1
bluetooth.h			GPIOA PIN2/3
AD.h				GPIOA PIN4/5 ADC1_CH4/5
Pwm_2.h				GPIOA PIN6/7 TIM3_CH1/2 20hz
Pwm_1.h				GPIOA PIN8 TIM1_CH1 50hz
usart.h				GPIOA PIN9/10 TX/RX Black/White
ray.h				GPIOA PIN11/12 GPIOB PIN0/1/5
dc_motor.h			GPIOB PIN3/4/6/7 TIM4_CH1/2
pca9685.h			GPIOB PIN8/9 I2C_1 
mpu6050.h			GPIOB PIN10/11 I2C_2
cs100a.h			GPIOB PIN12/13 
beep.h				GPIOB PIN14
led.h				GPIOB PIN15
Timer.h				TIM2 10hz
*/

//********************************************全局变量声明********************************************
extern float Voltage[2];	//ad测量电压值[0.3.3]			//ad.c
extern int Duty;			//直流电机pwm波占空比[0,500]		//dc_motor.c
extern float distance;		//超声波测距值					//cs100a.c
extern float Speed_Cm_S;	//小车速度cm/s					//dc_motor.c
extern char  USART_RX_INFO[USART_REC_LEN];	//uart接收数据	//usart.c
extern int x,y;												//PCA9685.c

int16_t Ax,Ay,Az,Pitch,Roll,Yaw;			//mpu6050接收到的数据
int order=0;				//蓝牙接收到的命令
int duty;					//占空比设定值

//*********************************************主函数部分*********************************************
int main(void)
{	
	//********************初始化程序********************
	BlueToothInit(9600,USART_Parity_No,USART_StopBits_1,USART_WordLength_8b);	//蓝牙初始化
	OLED_Init();				//oled初始化
	Beep_Init();				//蜂鸣器初始化
	Led_Init();					//led灯初始化
	Ray_Init();					//红外模块初始化
	MPU6050_Init();				//mpu6050初始化
	uart_init(115200);			//uart1初始化
	Encoder_Init();				//直流电机编码器初始化
	AD_Init();					//ad初始化
	Timer_Init();				//0.1s定时器初始化
	Cs100a_Init();				//超声波测距初始化
	PCA9685_Init();				//PCA9685初始化
	PWM_Init();					//直流电机pwm初始化
	TIM1_PWM_Init(9999,143);	//一周期20ms，分辨率20ms/10000）
	TIM_SetCompare1(TIM1,CENTRE_ANGLE);	//对齐角度为90度(1.5ms)
	Dc_Motor_Init();			//直流电机初始化（必须是最后配置的GPIO！）

	
	//*******************************************测试程序部分*******************************************
	

	//简单遥控功能实现
	int angle=0;
	int Length=0;
	float speed=0;
	
//	Dc_Motor_Move(20,0);
	
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
			case 0:Dc_Motor_Stop();break;
			case 1:Dc_Motor_Go();break;
			case 2:Dc_Motor_Back();break;
			case 3:duty=duty+10;Dc_Motor_SetDuty(duty);break;
			case 4:duty=duty-10;Dc_Motor_SetDuty(duty);break;
			case 5:Length+=5;Dc_Motor_Move(Length,0);break;
			case 6:Length-=5;Dc_Motor_Move(Length,0);break;
			case 7:angle+=10;Set_Angle(angle);break;
			case 8:angle-=10;Set_Angle(angle);break;
			case 9:angle=0;	Set_Angle(angle);break;	
			case 10:speed+=10;Dc_Motor_SetSpeed_PID(speed);break;
			case 11:speed-=10;Dc_Motor_SetSpeed_PID(speed);break;
			case 12:Turn_Left_90();break;
			case 13:Turn_Right_90();break;
			case 14:Fast_Turn_Left_90();break;
			case 15:Fast_Turn_Right_90();break;
		}	
		order=-1;
	}


/*
	//简单避障功能实现
	while(1)
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
*/

/*
	//mpu6050测试
	while(1)
	{
		MPU6050_GetData(&Ax,&Ay,&Az,&Pitch,&Roll,&Yaw);
		if(Ax==0||Ax==-11823)MPU6050_Init();			//mpu6050出错重启
		BlueTooth_TransmitPrintf("Ax:%d\r\nAy:%d\r\nAz:%d\r\npitch:%d\r\nrow:%d\r\nyaw:%d\r\n",Ax,Ay,Az,Pitch,Roll,Yaw);
	}
*/

	//简单寻迹，追点功能测试
	while(1)
	{
		//蓝牙接收信号处理部分
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
			BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
			BlueTooth_TransmitPrintf("\r\nOK\r\n");
			order=BlueTooth_RxPack[0];
		}
		switch(order){
			case 0:Dc_Motor_Stop();break;
			case 1:Follow_Track(50);break;
			case 2:Follow_Point();break;
			case 3:JoyStick_Control();break;
			case 4:;break;
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
		}	
		order=-1;
	}

}

//*********************************************中断函数部分*********************************************
//0.1s一次中断(oled状态刷新)	
void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{	
		//基本状态显示
		Cs100a_Start();
		Show_Voltage_State();
		Show_DC_Motor_State();	
		Show_Distance();
		
		//显示红外传感器状态
		OLED_ShowNum(2,11,L_2,1);
		OLED_ShowNum(2,12,L_1,1);
		OLED_ShowNum(2,13,C,1);
		OLED_ShowNum(2,14,R_1,1);
		OLED_ShowNum(2,15,R_2,1);
		
		//uart接收处理显示
		Get_Point_Pos();
		OLED_ShowNum(3,12,x,3);
		//OLED_ShowString(3,12,USART_RX_INFO);
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		//显示当前角度
//		static int times=0;
//		times++;
//		if(times==50)
//		{
//			times=0;
//			MPU6050_GetData(&Ax,&Ay,&Az,&Pitch,&Roll,&Yaw);
//			if(Ax==0||Ax==-11823)MPU6050_Init();			//mpu6050出错重启
//			BlueTooth_TransmitPrintf("Ax:%d\r\nAy:%d\r\nAz:%d\r\npitch:%d\r\nrow:%d\r\nyaw:%d\r\n",Ax,Ay,Az,Pitch,Roll,Yaw);
//		}


	}
}

//超声波模块中断函数
void EXTI15_10_IRQHandler()
{
	static int temp;
	int counter;
	//在cs100a的echo引脚发送脉冲信号的上升沿和下降沿触发
	if (EXTI_GetITStatus(EXTI_Line13) == SET)
	{
		counter=TIM_GetCounter(TIM2);
		
		//确保下降沿计数值比上升沿大，相减即可实现间隔时间的测量，从而不用使用while循环等待下降沿到来
		if(counter>temp)
		{
			time=counter-temp;
			temp=counter;
		}
		else temp=counter;
		
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
