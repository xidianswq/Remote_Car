#include "dma.h"

void BlueToothDmaInit(uint8_t *DMA_MemoryBaseAddr)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_InitTypeDef BlueToothDmaInitStructure;
	BlueToothDmaInitStructure.DMA_PeripheralBaseAddr=(uint32_t)&USART2->DR;			//外设站点起始地址
	BlueToothDmaInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;	//数据宽度
	BlueToothDmaInitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;			//是否自增
	BlueToothDmaInitStructure.DMA_MemoryBaseAddr=(uint32_t)DMA_MemoryBaseAddr;		//存储器站点起始地址
	BlueToothDmaInitStructure.DMA_MemoryDataSize=DMA_PeripheralDataSize_Byte;		//数据宽度
	BlueToothDmaInitStructure.DMA_MemoryInc=DMA_PeripheralInc_Enable;				//是否自增
	BlueToothDmaInitStructure.DMA_Mode=DMA_Mode_Circular;							//是否使用自动重装
	BlueToothDmaInitStructure.DMA_BufferSize=1;										//缓存区大小（传输计数器）
	BlueToothDmaInitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;						//传输方向
	BlueToothDmaInitStructure.DMA_M2M=DMA_MemoryInc_Disable;						//硬件触发还是软件
	BlueToothDmaInitStructure.DMA_Priority=DMA_Priority_High;						//优先级
	
	DMA_Init(DMA1_Channel6,&BlueToothDmaInitStructure);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA1_Channel6,ENABLE);
}

void Uart1DmaInit(uint8_t *DMA_MemoryBaseAddr,uint32_t DMA_BufferSize)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_InitTypeDef UartDmaInitStructure;
	UartDmaInitStructure.DMA_PeripheralBaseAddr=(uint32_t)&USART2->DR;			//外设站点起始地址
	UartDmaInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;	//数据宽度
	UartDmaInitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;			//是否自增
	UartDmaInitStructure.DMA_MemoryBaseAddr=(uint32_t)DMA_MemoryBaseAddr;		//存储器站点起始地址
	UartDmaInitStructure.DMA_MemoryDataSize=DMA_PeripheralDataSize_Byte;		//数据宽度
	UartDmaInitStructure.DMA_MemoryInc=DMA_PeripheralInc_Enable;				//是否自增
	UartDmaInitStructure.DMA_Mode=DMA_Mode_Circular;							//是否使用自动重装
	UartDmaInitStructure.DMA_BufferSize=DMA_BufferSize;							//缓存区大小（传输计数器）
	UartDmaInitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;							//传输方向
	UartDmaInitStructure.DMA_M2M=DMA_MemoryInc_Disable;							//硬件触发还是软件
	UartDmaInitStructure.DMA_Priority=DMA_Priority_High;						//优先级
	
	DMA_Init(DMA1_Channel5,&UartDmaInitStructure);
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA1_Channel5,ENABLE);
}
