//********************************************引脚使用说明********************************************
/*
oled.h				GPIOA PIN0/1
bluetooth.h			GPIOA PIN2/3
joystick.h			GPIOA PIN4/5 ADC1_CH4/5 GPIOB PIN11/12/13 EXTI12/13
Pwm.h				GPIOA PIN8/11 TIM1_CH1/4 50hz
usart.h				GPIOA PIN9/10 TX/RX Black/White
beep.h				GPIOB PIN14
led.h				GPIOB PIN15
Timer.h				TIM2/3
*/

//*********************************************头文件声明*********************************************
#include "public.h"				//公用引用函数封装
//#include "bluetooth.h"		//蓝牙模块
#include "oled.h"				//OLED显示屏模块
#include "Pwm.h"				//PWM波生成模块
#include "servo_motor.h"		//云台控制函数模块
#include "joystick.h"			//摇杆控制模块
#include "string.h"				
#include "Delay.h"				
#include "Timer.h"				//定时器模块
#include "usart.h"				//uart通信模块
#include "beep.h"				//蜂鸣器模块
#include "led.h"				//led灯模块
#include "dma.h"				//dma数据转存模块

//*****************************************全局变量和宏定义声明****************************************
//#define OpenLoop_OL		//开环实现功能执行
#define CloseLoop_CL		//闭环实现功能执行

extern float Voltage[2];	//ad测量电压值[0.3.3]			//ad.c
extern char  USART_RX_INFO[USART_REC_LEN];	//uart接收数据	//usart.c
extern int x,y;				//激光当前坐标					//servo_motor.c
extern int Vertex[4][2];	//四顶点位置						//servo_motor.c
extern int Vertex_Peak_Pos[4][2];
extern int Vertex_A4[4][2];
extern Pwm Center_Pwm;
extern Pwm Peak_Pwm[4];
extern Pwm A4_Pwm[4];

int Programme_Progress=0;					//比赛程序进度
int order=0;								//蓝牙接收到的命令
int Main_Wait_Stop_Sign =1;					//主程序等待标志位
extern int JoyStick_Control_Stop_Sign;		//摇杆控制程序结束标志位
int Get_Depend_Point_Pos_Stop_Sign=1;
int Get_A4_Point_Pos_Stop_Sign=1;
extern int Follow_Track_Stop_Sign;			//矩形寻迹结束标志位
extern int Follow_Point_Stop_Sign;			//绿激光跟随红激光结束标志位


//*********************************************主函数部分*********************************************
//重新重启初值还原设置
void Programme_Reset(void)
{
	Beep_Times(1000,1,NORMAL_MODE);
	Led_Times(1000,1,NORMAL_MODE);
	Programme_Progress=0;
	
	Main_Wait_Stop_Sign=1;
	JoyStick_Control_Stop_Sign=1;
	Follow_Track_Stop_Sign=1;
	
	Get_A4_Point_Pos_Stop_Sign=1;
	Get_Depend_Point_Pos_Stop_Sign=1;
}

int main(void)
{	
	//********************初始化程序********************
	Timer_Init();				//定时器初始化
//	BlueToothInit(9600,USART_Parity_No,USART_StopBits_1,USART_WordLength_8b);	//蓝牙初始化
	OLED_Init();				//oled初始化
	Beep_Init();				//蜂鸣器初始化
	Led_Init();					//led灯初始化
	TIM1_PWM_Init(9999,143);	//一周期20ms，分辨率20ms/10000）
	TIM_SetCompare1(TIM1,750);	//对齐角度为90度(1.5ms)
	TIM_SetCompare4(TIM1,763);	//对齐角度为90度(1.5ms)
	uart_init(115200);			//uart1初始化
	JoyStick_Init();			//JoyStick摇杆初始化
			
	//*******************************************比赛程序部分*******************************************
	while(1)
	{
		int i;
		//重新重启初值还原设置
		Programme_Reset();
//		Reach_Pos_CL(50,50,PID_MODE);
		
		Axes_Init();
		
//		Follow_Track(Vertex_Peak_Pos,1);
		
		while(Main_Wait_Stop_Sign);
		//摇杆控制
		JoyStick_Control();
	
		
//#ifdef OpenLoop_OL
//			Follow_Track_OL();
//#endif			
//#ifdef CloseLoop_CL
//		//等待上位机发送初始坐标
//		Get_Depend_Point_Pos();
//		//环绕正方形顺时针旋转一周
//		while(Get_Depend_Point_Pos_Stop_Sign);

		//Follow_Track_CL(Vertex_Peak_Pos,2,PID_MODE);
		
//#endif

		Pwm_Track(Peak_Pwm,1);
		while(Follow_Track_Stop_Sign);
				
		Get_A4_Point_Pos();
		Timer_delay_ms(2000);
//		Follow_Track_CL(Vertex_A4,4,MINMIZE_MODE);
//		Follow_Track(Vertex_A4,4);
		for(i=0;i<4;i++)Get_Pwm(Vertex_A4[i][0],Vertex_A4[i][1],&A4_Pwm[i],1);
		Pwm_Track(A4_Pwm,6);
		while(Get_A4_Point_Pos_Stop_Sign);
		
	}
}

//*********************************************中断函数部分*********************************************
//按键中断函数
void EXTI15_10_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		EXIT_LINE_Timer_delay_ms(10);										
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)		//软件防抖
		{
			Beep_Times(50,2,EXIT_LINE_MODE);
			Reach_Pos_OL(Oc_L,Oc_V);						//保持激光当前指向位置
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0); 	//等待按键松开

			//再次按下才退出
			EXIT_LINE_Timer_delay_ms(10);
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==1);		
			EXIT_LINE_Timer_delay_ms(10);										
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0);		//软件防抖
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11==1)); 	//等待按键松开
			Beep_Times(50,2,EXIT_LINE_MODE);
			
			EXTI_ClearITPendingBit(EXTI_Line11);
		}
	}
	
	else if (EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		EXIT_LINE_Timer_delay_ms(10);										
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)		//软件防抖
		{
			Programme_Progress++;
			Beep_Times(500,1,EXIT_LINE_MODE);
			if(Programme_Progress==1)
			{
				Main_Wait_Stop_Sign=0;
			}
			else if(Programme_Progress==2)
			{
				JoyStick_Control_Stop_Sign=0;
			}
			else if(Programme_Progress==3)
			{
//				Get_Depend_Point_Pos_Stop_Sign=0;
				Follow_Track_Stop_Sign=0;
			}
			else if(Programme_Progress==4)
			{
				Get_A4_Point_Pos_Stop_Sign=0;
//				Follow_Track_Stop_Sign=0;
			}
			else if(Programme_Progress==5)
			{
//				Get_A4_Point_Pos_Stop_Sign=0;
			}
			else if(Programme_Progress==6)
			{
				;
			}
			else if(Programme_Progress==7)
			{
				;
			}
			else
			{
				Programme_Reset();
			}
			
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0); 	//等待按键松开
			EXTI_ClearITPendingBit(EXTI_Line12);
		}
		
	}
	
	else if (EXTI_GetITStatus(EXTI_Line13) == SET)
	{
		EXIT_LINE_Timer_delay_ms(10);										
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0)		//软件防抖
		{
			Beep_Times(50,3,EXIT_LINE_MODE);
			
			Reach_Pos_OL(Center_Pwm.level,Center_Pwm.vert);
			
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0); 	//等待按键松开

			//再次按下才退出
			EXIT_LINE_Timer_delay_ms(10);
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==1);		
			EXIT_LINE_Timer_delay_ms(10);										
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0);		//软件防抖
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13==1)); 	//等待按键松开
			Beep_Times(50,3,EXIT_LINE_MODE);
			
			EXTI_ClearITPendingBit(EXTI_Line13);
		}
	}	
}
