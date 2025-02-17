#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"
#include "string.h"

#define USART_REC_LEN 64  					//定义最大接收字节数 64

extern const char head_tag;					//标志头
extern const char tail_tag;					//标志尾
extern uint8_t RxData;
extern uint16_t RxPackFlag;
extern char USART_RX_INFO[USART_REC_LEN];	//接收数据缓存（不包括标识头尾）

extern int p_x,p_y,p_len;							//跟随人脸当前坐标及距离

void uart_init(u32 bound);
void CLR_Buf(void);

#endif
