/**********************************************************
												
接线说明：
STM32					GPS
VCC		------>	VCC
GND		------>	GND
RX1		<------	TXD
PA10
STM32					USB-TTL模块
GND		------>	GND
TX1		------>	RXD
PA9
***********************************************************/

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "ATGM.h"


int main(void)
{	
	delay_init();
	
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为9600
	
	clrStruct();
	
	printf("Welcome to use!\r\n");
	while(1)
	{
		parseGpsBuffer();
		printGpsBuffer();
	}
}





