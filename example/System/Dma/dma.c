#include "dma.h"

void BlueToothDmaInit(uint8_t *DMA_MemoryBaseAddr)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_InitTypeDef BlueToothDmaInitStructure;
	BlueToothDmaInitStructure.DMA_PeripheralBaseAddr=(uint32_t)&USART2->DR;
	BlueToothDmaInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	BlueToothDmaInitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	BlueToothDmaInitStructure.DMA_MemoryBaseAddr=(uint32_t)DMA_MemoryBaseAddr;
	BlueToothDmaInitStructure.DMA_MemoryDataSize=DMA_PeripheralDataSize_Byte;
	BlueToothDmaInitStructure.DMA_MemoryInc=DMA_PeripheralInc_Enable;
	BlueToothDmaInitStructure.DMA_Mode=DMA_Mode_Circular;
	BlueToothDmaInitStructure.DMA_BufferSize=1;
	BlueToothDmaInitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	BlueToothDmaInitStructure.DMA_M2M=DMA_MemoryInc_Disable;
	BlueToothDmaInitStructure.DMA_Priority=DMA_Priority_Medium;
	DMA_Init(DMA1_Channel6,&BlueToothDmaInitStructure);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	DMA_Cmd(DMA1_Channel6,ENABLE);
}

