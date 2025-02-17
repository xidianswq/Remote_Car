#ifndef __SERVO_MOTOR_H
#define __SERVO_MOTOR_H
#include "public.h"
#include "Pwm.h"
#include "usart.h"
#include "oled.h"
#include "pid.h"
#include "beep.h"

//水平方向舵机Level--PWM1_CH1 竖直方向舵机Vert--PWM1_CH4

#define SERVO000	250 		//0度对应脉宽计数值
#define SERVO090    750			//90度对应脉宽计数值
#define SERVO180	1250 		//180度对应脉宽计算值
#define SERVOSUB	1000		//0度到90度脉宽差		
#define SERVOADJUST 750			//矫正初值使水平(安装时误差)
#define SERVOUP 	800
#define SERVODOWN 	650

extern int Oc_L;
extern int Oc_V;
extern int Vertex[4][2];
extern int OrginPos_Pos_L;		//正方形中心水平Level方向坐标
extern int OrginPos_Pos_V;		//正方形中心竖直Vert方向坐标
extern int LeftUpPos_Pos_L;		//正方形左上角水平Level方向坐标
extern int LeftUpPos_Pos_V; 	//正方形左上角竖直Vert方向坐标
extern int RightDownPos_Pos_L;		
extern int RightDownPos_Pos_V;
extern int Red_x;
extern int Red_y;
extern int Follow_Point_Stop_Sign;

void Spinnig_Level(int diff);
void Spinnig_Vert(int diff);
//void Set_Angle_Vert(float angle);
void JoyStick_Control(void);
void Show_Pwm_Compare(void);

//开环控制函数
void Reach_Pos_OL(int Pos_Pwm_L,int Pos_Pwm_V);
void Restart_Pos_OL(void);
void Follow_Track_OL(void);

//闭环控制函数
typedef enum{
	PID_MODE=0,
	MINMIZE_MODE=1
}Reach_Pos_CL_MODE;
void Get_Depend_Point_Pos(void);
void Get_Point_Pos(void);
void Reach_Pos_CL(int Target_X,int Target_Y,int Reach_Pos_CL_MODE);
int Sign(int num);
int my_abs(int a,int b);
int near(int Target_X,int Target_Y);
void Restart_Pos_CL(int Reach_Pos_CL_MODE);
void Follow_Track_CL(int Vertex[4][2],int divide_num,int Reach_Pos_CL_MODE);
void Get_A4_Point_Pos(void);
void Follow_Point(void);
void Get_Red_Point_Pos(void);
	
void Reach_LeftUPPos(void);
void Reach_OrginPos(void);



typedef struct Pwm{
	int level;
	int vert;
}Pwm;
extern int Vertex_Peak_Pos[4][2];
extern int Center_Pos[2];
extern Pwm Center_Pwm;
extern Pwm Peak_Pwm[4];
extern Pwm A4_Pwm[4];

void Get_Pwm(int px,int py,Pwm *target_pwm,int n);
void Axes_Init(void);
void Get_Point_5(void);
void Follow_Track(int Vertex[4][2],int divide_num);
void Pwm_Track(Pwm Vertex_Pwm[4],int divide_num);


#endif

