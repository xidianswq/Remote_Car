#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H
#include "public.h"
#define HEXPACKMODE
//#define TEXTPACKMODE
#define PACKLENGTH 1

extern uint8_t BlueTooth_TxData;
extern uint8_t BlueTooth_RxData;
extern uint8_t BlueTooth_TxPack[PACKLENGTH];
extern uint8_t BlueTooth_RxPack[PACKLENGTH];
extern uint16_t BlueTooth_RxPackFlag;

void BlueToothInit(uint16_t BaudRate,uint16_t USART_Parity,uint16_t USART_StopBits,uint16_t USART_WordLength);
uint8_t *Get_BlueTooth_ReceivePack(void);
uint16_t Get_BlueTooth_ReceivePackFlag(void);
void BlueTooth_TransmitByte(uint8_t BlueTooth_TransmitData);
void BlueTooth_TransmitString(char *String);
void BlueTooth_TransmitPrintf(char *format, ...);

#endif
