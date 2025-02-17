#include "bluetooth.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

uint8_t BlueTooth_TxData;
uint8_t BlueTooth_RxData;
uint8_t BlueTooth_TxPack[PACKLENGTH];
uint8_t BlueTooth_RxPack[PACKLENGTH];
uint16_t BlueTooth_RxPackFlag;

/**
  * @brief  Open the bluetooth function onUSART2
  */
void BlueToothInit(uint16_t BaudRate,uint16_t USART_Parity,uint16_t USART_StopBits,uint16_t USART_WordLength)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef BlueTooth_GPIO_Tx_InitStructure;
	BlueTooth_GPIO_Tx_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	BlueTooth_GPIO_Tx_InitStructure.GPIO_Pin=GPIO_Pin_2;
	BlueTooth_GPIO_Tx_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&BlueTooth_GPIO_Tx_InitStructure);
	
	GPIO_InitTypeDef BlueTooth_GPIO_Rx_InitStructure;
	BlueTooth_GPIO_Rx_InitStructure.GPIO_Mode=GPIO_Mode_IPU;				//Use GPIO_Mode_AF_PP!
	BlueTooth_GPIO_Rx_InitStructure.GPIO_Pin=GPIO_Pin_3;
	BlueTooth_GPIO_Rx_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&BlueTooth_GPIO_Rx_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=BaudRate;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity;
	USART_InitStructure.USART_StopBits=USART_StopBits;
	USART_InitStructure.USART_WordLength=USART_WordLength;
	USART_Init(USART2,&USART_InitStructure);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_USART2_InitStructure;
	NVIC_USART2_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_USART2_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_USART2_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_USART2_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_USART2_InitStructure); 
	
	USART_Cmd(USART2,ENABLE);
}

void BlueTooth_TransmitByte(uint8_t BlueTooth_TransmitData)
{
	USART_SendData(USART2,BlueTooth_TransmitData);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

uint8_t *Get_BlueTooth_ReceivePack(void)
{
	return BlueTooth_RxPack;
}

uint16_t Get_BlueTooth_ReceivePackFlag(void)
{
	if(BlueTooth_RxPackFlag==1)
	{
		BlueTooth_RxPackFlag=0;
		return 1;
	}
	return 0;
	
}

int fputc(int ch, FILE *f)
{
	BlueTooth_TransmitByte(ch);
	return ch;
}

void BlueTooth_TransmitString(char *String)
{
	uint16_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		BlueTooth_TransmitByte(String[i]);
	}
}

void BlueTooth_TransmitPrintf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	BlueTooth_TransmitString(String);
}

void USART2_IRQHandler(void)
{
	static uint16_t BlueTooth_RxPackState=0;
	static uint16_t BlueTooth_RxPackPoint=0;
	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)==SET)
	{
		BlueTooth_RxData=USART_ReceiveData(USART2);
		
#ifdef HEXPACKMODE	
		if(BlueTooth_RxPackState==0)
		{
			if(BlueTooth_RxData==0xFF)
			{
				BlueTooth_RxPackState=1;
				BlueTooth_RxPackPoint=0;
			}
		}
		else if(BlueTooth_RxPackState==1)
		{
			BlueTooth_RxPack[BlueTooth_RxPackPoint++]=BlueTooth_RxData;
			if(BlueTooth_RxPackPoint>=PACKLENGTH)
			{
				BlueTooth_RxPackState=2;
			}
		}
		else if(BlueTooth_RxPackState==2)
		{
			if (BlueTooth_RxData == 0xFE)
			{
				BlueTooth_RxPackState = 0;
				BlueTooth_RxPackFlag = 1;
			}
		}
#endif
		
#ifdef TEXTPACKMODE
		if(BlueTooth_RxPackState==0)
		{
			static uint16_t Cmd_Point=0;
			if(BlueTooth_RxData=='C'&& Cmd_Point==0)
			{
				Cmd_Point=1;
			}
			else if(BlueTooth_RxData=='M'&& Cmd_Point==1)
			{
				Cmd_Point=2;
			}
			else if(BlueTooth_RxData=='D'&& Cmd_Point==2)
			{
				Cmd_Point=3;
			}
			else if(BlueTooth_RxData=='+'&& Cmd_Point==3)
			{
				Cmd_Point=0;
				BlueTooth_RxPackState=1;
				BlueTooth_RxPackPoint=0;
			}	
			else Cmd_Point=0;
		}
		else if(BlueTooth_RxPackState==1)
		{
			if (BlueTooth_RxData == '!')
			{
				BlueTooth_RxPackState = 0;
				BlueTooth_RxPackFlag = 1;
				BlueTooth_RxPack[BlueTooth_RxPackPoint]='\0';
			}
			else BlueTooth_RxPack[BlueTooth_RxPackPoint++]=BlueTooth_RxData;
		}
#endif
		
	}
}
