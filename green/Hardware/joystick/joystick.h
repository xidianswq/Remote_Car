#ifndef __JOYSTICK_H
#define __JOYSTICK_H
#include "public.h"
#include "oled.h"

extern uint16_t ADValue[2];	
extern float Voltage[2];
extern int JoyStick_Control_Stop_Sign;

void AD_Init(void);
//uint16_t AD_GetValue(void);
void Show_Voltage_State(void);
void JoyStick_Init(void);

#endif
