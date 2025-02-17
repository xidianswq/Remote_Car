#include "servo_motor.h"

#define SIGN 1					//方便调转向
#define MAX_ANGLE 60			//最大转向角

void Set_Angle(float angle)
{
	if(angle>MAX_ANGLE)angle=MAX_ANGLE;
	else if(angle<-MAX_ANGLE)angle=-MAX_ANGLE;
	TIM_SetCompare1(TIM1,710+SIGN*angle*50/9);
}

//长距离转弯（无后撤）
void Turn_Left_90(void)
{
	Set_Angle(-60);
	delay_ms(10);		//防止没转过来小车就开始行驶
	Dc_Motor_Move(23,30);
	Set_Angle(0);
	Beep_Times(3,1);
}

void Turn_Right_90(void)
{
	Set_Angle(60);
	delay_ms(10);		//防止没转过来小车就开始行驶
	Dc_Motor_Move(52,30);
	Set_Angle(0);
	Beep_Times(3,1);
}


//短距离转弯（有后撤）
void Fast_Turn_Left_90(void)
{
	Set_Angle(-60);
	delay_ms(10);		//防止没转过来小车就开始行驶
	Dc_Motor_Move(15,50);
	
	Set_Angle(0);
	delay_ms(10);
	
	Set_Angle(60);
	delay_ms(10);		//防止没转过来小车就开始行驶
	Dc_Motor_Move(-23,30);
	
	Set_Angle(0);
	Beep_Times(3,1);
}

void Fast_Turn_Right_90(void)
{
	Set_Angle(60);
	delay_ms(20);		//防止没转过来小车就开始行驶
	Dc_Motor_Move(30,30);
	
	Set_Angle(0);
	delay_ms(10);
	
	Set_Angle(-60);
	delay_ms(10);		//防止没转过来小车就开始行驶
	Dc_Motor_Move(-14.5,50);
	
	Set_Angle(0);
	Beep_Times(3,1);
}
