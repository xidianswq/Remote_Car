#include "public.h"
#include "shake.h"
#include "testled.h"
#include "bluetooth.h"

#include "atgm_usart.h"
#include "atmg.h"

#include "string.h"		
#include "time.h"
#include "mpu6050.h"

//#include "dma.h"

//uint8_t Rx_Date[500];
extern float Pitch,Roll,Yaw;

int main(void)
{
	//初始化程序
	//BLUETOOTH
	BlueToothInit(9600,USART_Parity_No,USART_StopBits_1,USART_WordLength_8b);
	//GPS
	atgm_uart_init(9600);	 //串口初始化为9600
	//SHAKE
	ShakeInit();
	//MPU6050
		//iic
		IIC_Init();
		MPU6050_initialize();     //=====MPU6050初始化	
		DMP_Init();
		TIM2_Getsample_Int(1999,719);		//50ms任务定时中断
	
	//主程序
	while(1)
	{
		if(Get_BlueTooth_ReceivePackFlag()==1)
		{
			//read gps data
			BlueTooth_TransmitPrintf("%s\r\n",BlueTooth_RxPack);
			BlueTooth_TransmitPrintf("\r\nGPS INFORMATION:\r\n");
			parseGpsBuffer();
			printGpsBuffer();
			BlueTooth_TransmitPrintf("\r\nTILT ANGLE INFORMATION:\r\n");
			BlueTooth_TransmitPrintf("Pitch:%.2f\r\n",Pitch);
			BlueTooth_TransmitPrintf("Roll :%.2f\r\n",Roll);
			BlueTooth_TransmitPrintf("Yaw  :%.2f\r\n",Yaw);
			BlueTooth_TransmitPrintf("\r\nOK\r\n");
		}
	}
}
