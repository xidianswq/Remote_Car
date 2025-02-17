#include "usart.h"	
#include "pca9685.h"
//黑-A9-TX 白-A10-RX
u16 point1 = 0;
   	
extern int x,y;							//杂草坐标	//PCA9685.c	
	
char USART_RX_INFO[USART_REC_LEN];	  	//接收数据缓存（不包括标识头尾）
const char head_tag = '#';
const char tail_tag = '$';
uint8_t RxData;
uint16_t RxPackFlag;
  
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 

	CLR_Buf();//清空缓存
}

void USART1_IRQHandler(void)                				//串口1中断服务程序
{
	static uint16_t RxPackState=0;
	static uint16_t RxPackPoint=0;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) 
	{
		RxData =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if(RxPackState == 0)
		{
			static uint16_t Cmd_Point=0;
			if(RxData == head_tag && Cmd_Point == 0)
			{	
				Cmd_Point=1;
				RxPackState=1;
				RxPackPoint=0;
			}	
			else Cmd_Point=0;
		}
		else if(RxPackState == 1)
		{
			if (RxData == tail_tag)
			{
				RxPackState = 0;
				RxPackFlag = 1;
				USART_RX_INFO[RxPackPoint]='\0';
			}
			else USART_RX_INFO[RxPackPoint++]=RxData;
		}
	}
}

void CLR_Buf(void)                           				// 串口缓存清理
{
	memset(USART_RX_INFO, 0, USART_REC_LEN);      			//清空
  point1 = 0;                    
}
