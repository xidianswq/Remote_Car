#ifndef DC_MOTOR_H
#define DC_MOTOR_H
#include "public.h"


#define In1_On GPIO_SetBits(GPIOA,GPIO_Pin_12) 
#define In1_Off	GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define In2_On GPIO_SetBits(GPIOA,GPIO_Pin_11) 
#define In2_Off	GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define In3_On GPIO_SetBits(GPIOA,GPIO_Pin_10) 
#define In3_Off	GPIO_ResetBits(GPIOA,GPIO_Pin_10)
#define In4_On GPIO_SetBits(GPIOA,GPIO_Pin_9) 
#define In4_Off	GPIO_ResetBits(GPIOA,GPIO_Pin_9)

void Dc_Motor_Init(void);
void Dc_Motor_Stop(void);
void Dc_Motor_Go(void);
void Dc_Motor_Back(void);
void Dc_Motor_Left(void);
void Dc_Motor_Right(void);

#endif
