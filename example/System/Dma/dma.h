#ifndef _DMA_H
#define _DMA_H
#include "public.h"

void BlueToothDmaInit(uint8_t *DMA_MemoryBaseAddr);
void Uart1DmaInit(uint8_t *DMA_MemoryBaseAddr,uint32_t DMA_BufferSize);

#endif
