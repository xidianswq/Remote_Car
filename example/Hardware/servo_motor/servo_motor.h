#ifndef __SERVO_MOTOR_H
#define __SERVO_MOTOR_H
#include "public.h"
#include "Pwm_1.h"
#include "dc_motor.h"
#include "beep.h"

void Set_Angle(float angle);
void Turn_Left_90(void);
void Turn_Right_90(void);
void Fast_Turn_Left_90(void);
void Fast_Turn_Right_90(void);

#endif
