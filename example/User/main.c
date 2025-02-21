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
#include "beep.h"
#include "led.h"
#include "ray.h"
#include "pca9685.h"
#include "red_ray.h"

//********************************************引脚使用说明********************************************
/*
oled.h				GPIOA PIN0/1
bluetooth.h			GPIOA PIN2/3
AD.h				GPIOA PIN4/5 ADC1_CH4/5
Pwm_2.h				GPIOA PIN6/7 TIM3_CH1/2 20hz
Pwm_1.h				GPIOA PIN8 TIM1_CH1 50hz
usart.h				GPIOA PIN9/10 TX/RX Black/White
dc_motor.h			GPIOB PIN3/4/6/7 TIM4_CH1/2
pca9685.h			GPIOB PIN8/9 I2C_1(SCL/SDA) 
beep.h				GPIOB PIN14
led.h				GPIOB PIN15
Timer.h				TIM2 10hz
*/

//********************************************全局变量声明********************************************
extern float Voltage[2];	//ad测量电压值[0.3.3]			//ad.c
extern int Duty;			//直流电机pwm波占空比[0,500]		//dc_motor.c
extern float Speed_Cm_S;	//小车速度cm/s					//dc_motor.c
extern char  USART_RX_INFO[USART_REC_LEN];	//uart接收数据	//usart.c
extern uint16_t RxPackFlag;					//uart接收标志位	//usart.c
extern int x,y;												//PCA9685.c
extern unsigned long long system_tick;	//系统节拍

int16_t Ax,Ay,Az,Pitch,Roll,Yaw;			//mpu6050接收到的数据
int mode=0;					//当前功能模式

void system_init(void){
	//********************初始化程序********************
	BlueToothInit(9600,USART_Parity_No,USART_StopBits_1,USART_WordLength_8b);	//蓝牙初始化
	OLED_Init();				//oled初始化
	Beep_Init();				//蜂鸣器初始化
	Led_Init();					//led灯初始化
	Red_Ray_Init();				//红色激光灯初始化
	uart_init(115200);			//uart1初始化
	Encoder_Init();				//直流电机编码器初始化
	AD_Init();					//ad初始化
	Timer_Init();				//0.1s定时器初始化
	PCA9685_Init();				//PCA9685初始化
	PWM_Init();					//直流电机pwm初始化
	TIM1_PWM_Init(9999,143);	//一周期20ms，分辨率20ms/10000）
	TIM_SetCompare1(TIM1,710);	//对齐角度为90度(1.5ms)
	Dc_Motor_Init();			//直流电机初始化（必须是最后配置的GPIO！）
	system_tick=0;
}

//********************************************激光除草小车********************************************
void ray_cvcar(void){
	unsigned long long system_tick_temp=0;
	int updata_sign=1;
	while(1)
	{
		//蓝牙接收信号处理部分
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
			BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
			mode=BlueTooth_RxPack[0];
			switch(mode){
				case 0:Dc_Motor_Stop();break;
				case 1:Dc_Motor_Go();break;
				case 2:Dc_Motor_SetDuty(100);break;
				case 3:Set_Angle(0);break;
				case 4:Set_Angle(-40);break;
				case 5:Set_Angle(40);break;
				case 6:Red_Ray_On();break;
				case 7:Red_Ray_Off();break;
				default:break;
			}
		}
		
		if(RxPackFlag==1){
			if(updata_sign)
			{
				system_tick_temp=system_tick;
				updata_sign=0;
				Dc_Motor_Stop();
			}
			if((system_tick-system_tick_temp)<=50)
			{
				Nano_SetPWM();
				Red_Ray_On();
			}
			else{
				updata_sign=1;
				Red_Ray_Off();
				Dc_Motor_Go();
			}
			RxPackFlag=0;
		}
	}
}

//*********************************************主函数部分*********************************************
int main(void)
{	
	system_init();
	ray_cvcar();
}

//*********************************************中断函数部分*********************************************
//0.1s一次中断(oled状态刷新)	
void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{		
		
		system_tick++;
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	}
}

