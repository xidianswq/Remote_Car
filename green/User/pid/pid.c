#include "pid.h"

//速度环控制
int Pid_Control(float kp,float ki,float kd,int Target_Num,int Current_Num,int PID_Mode)
{
	float Pid_Num=0;		//返回的控制值
	
	static float Error_k=0;
	static float Error_k1=0;
	static float Error_k2=0;
	
	Error_k=Target_Num-Current_Num;
	Error_k1=Error_k-Error_k1;
	
	if(PID_Mode==PID_INCREASE)
	{
		Error_k2=Error_k-2*Error_k1+Error_k2;
		Pid_Num=kp*Error_k+ki*Error_k2+kd*Error_k1;
	}
	
	else if(PID_Mode==PID_REALIZE)
	{
		Pid_Num=kp*Error_k+kd*Error_k1;
	}

	Error_k2=Error_k1;
	Error_k1=Error_k;

	return (int)Pid_Num;
}

