#ifndef CS100A_H
#define CS100A_H
#include "public.h"
#include "oled.h"

extern int time;
extern float distance;

void Cs100a_Start(void);
void Cs100a_Init(void);
void Show_Distance(void);

#endif
