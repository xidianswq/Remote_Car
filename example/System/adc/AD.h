#ifndef __AD_H
#define __AD_H
#include "public.h"
#include "oled.h"

extern uint16_t ADValue[2];	
extern float Voltage[2];

void AD_Init(void);
//uint16_t AD_GetValue(void);
void Show_Voltage_State(void);

#endif
