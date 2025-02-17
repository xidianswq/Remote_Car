#ifndef __PCA9685_H
#define __PCA9685_H
#include "public.h"
#include "stdio.h"
#include "math.h"  
#include "led.h"
#include "pid.h"
#include "usart.h"

#define PCA9685_ADDRESS 0x80	//1+A5+A4+A3+A2+A1+A0+w/r 

#define PCA9685_SUBADR1 0x02
#define PCA9685_SUBADR2 0x03
#define PCA9685_SUBADR3 0x04

#define PCA9685_MODE1    0x00   //MODE1寄存器地址
#define PCA9685_PRESCALE 0xFE   //PRE_SCALE寄存器地址

#define LED0_ON_L  0x06			//通道0的四个控制寄存器
#define LED0_ON_H  0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09

#define ALLLED_ON_L  0xFA		//全部通道的四个控制寄存器
#define ALLLED_ON_H  0xFB		//舵机控制时一般不用
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

#define SERVO000	130 		//0度对应4096的脉宽计数值
#define SERVO090    325			//90度对应4096的脉宽计数值
#define SERVO180	520 		//180度对应4096的脉宽计算值
#define SERVO_L_RANGE	100		//限制水平旋转范围
#define SERVO_V_RANGE   150	 	//限制垂直旋转范围
#define SERVOSUB	390			//0度到90度的4096脉宽差		
#define SERVOADJUST 325			//矫正初值使水平(安装时误差)

extern int Off_L;				//水平舵机当前设定pwm的off值
extern int Off_V;				//竖直舵机当前设定pwm的off值

void PCA9685_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
void PCA9685_WriteReg(uint8_t RegAddress, uint8_t Data);
void PCA9685_Reset(void);
void PCA9685_SetPwmFreq(float freq);
void PCA9685_SetPwm(int num, int on, int off);
void PCA9685_Init(void);
void Spinnig_Level(int diff);
void Spinnig_Vert(int diff);
void Set_Angle_Vert(float angle);
void Follow_Point(void);
int Get_Point_Pos(void);
void JoyStick_Control(void);

#endif
