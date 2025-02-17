#ifndef __ATGM_H
#define __ATGM_H
#include "stdio.h"	
#include "stm32f10x.h"
#include "string.h"
#include "math.h"
#include "bluetooth.h"
#include "atgm_usart.h"

void errorLog(int num);
void parseGpsBuffer(void);
void printGpsBuffer(void);

#endif
