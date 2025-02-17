#ifndef _RAY_H
#define _RAY_H
#include "public.h"
#include "dc_motor.h"
#include "servo_motor.h"
#include "beep.h"

extern int L_2;
extern int L_1;
extern int C;
extern int R_1;
extern int R_2;
extern int Stop_Sign;

void Ray_Init(void);
void Get_Ray_State(void);
void Follow_Track(int Duty);


#endif
