#include "servo_motor.h"

#define SIGN 1					//方便调转向
#define MAX_ANGLE 360			//最大转向角

void Set_Angle_Level(float angle)
{
	if(angle>MAX_ANGLE)angle=MAX_ANGLE;
	else if(angle<-MAX_ANGLE)angle=-MAX_ANGLE;
	TIM_SetCompare2(TIM1,750+SIGN*angle*50/9);
}

void Set_Angle_Vert(float angle)
{
	if(angle>MAX_ANGLE)angle=MAX_ANGLE;
	else if(angle<-MAX_ANGLE)angle=-MAX_ANGLE;
	TIM_SetCompare3(TIM1,750+SIGN*angle*50/9);
}
