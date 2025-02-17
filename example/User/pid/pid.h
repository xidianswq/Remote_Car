#ifndef _PID_H
#define _PID_H
#include "public.h"

#define PID_REALIZE	0	//位置式PID
#define PID_INCREASE 1	//增量式PID

int Pid_Control(float kp,float ki,float kd,int Target_Num,int Current_Num,int PID_Mode);

#endif
