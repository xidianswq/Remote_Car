#ifndef DC_MOTOR_H
#define DC_MOTOR_H
#include "public.h"
#include "oled.h"
#include "Pwm_2.h"
#include "pid.h"
#include "bluetooth.h"
#include "beep.h"
#include "led.h"
#include "servo_motor.h"

//L298N的4个方向控制口连接两个端口，其中B9连IN1和IN3，B8连IN2和IN4
#define In1_On GPIO_SetBits(GPIOB,GPIO_Pin_3) 
#define In1_Off	GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define In2_On GPIO_SetBits(GPIOB,GPIO_Pin_4) 
#define In2_Off	GPIO_ResetBits(GPIOB,GPIO_Pin_4)

//#define In3_On GPIO_SetBits(GPIOB,GPIO_Pin_13) 
//#define In3_Off	GPIO_ResetBits(GPIOB,GPIO_Pin_13)

//#define In4_On GPIO_SetBits(GPIOB,GPIO_Pin_12) 
//#define In4_Off	GPIO_ResetBits(GPIOB,GPIO_Pin_12)

/*
轮子信息：
直径：6.7cm
周长：21.05cm
一圈产生脉冲数：1568
2cm脉冲数：149（在65532内，脉冲数完全够用）
精度：0.134mm（单脉冲距离）
*/
#define PER_PULSE 0.134		//mm
#define GEAR_RATIO 120.62	//减速比
#define PPR 13				//脉冲数每圈

void Dc_Motor_Init(void);
void Dc_Motor_Stop(void);
void Dc_Motor_Emerg_Stop(void);
void Dc_Motor_Go(void);
void Dc_Motor_Back(void);
void Dc_Motor_Left(void);
void Dc_Motor_Right(void);
void Dc_Motor_SetDuty(int);
void Dc_Motor_SetSpeed_PID(float Target_Speed_Cm_S);
void Dc_Motor_SetSpeed(float Target_Speed_Cm_S);
void Dc_Motor_Move(float Length_Cm,int Gate);
void Show_DC_Motor_State(void);
void Encoder_Init(void);
int16_t Encoder_Get(void);
float Speed_Get(int Time_S);
void Dc_Motor_Mode(void);			//遥控车功能
void Simple_Aviod_Obstacle(void);	//简单避障功能

#endif
