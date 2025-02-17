#include "joystick.h"

// GPIOA PIN4/5 ADC1_CH4/5

uint16_t AD_Value[2];
float Voltage[2]={0,0};

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//设置模拟输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//是否使用扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 2;			//扫描通道数
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//DMA初始化
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	//M2M是否使用软件触发
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	//外设使能
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	//复位校准寄存器
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

//不启用DMA时获取ADC数值
uint16_t AD_GetValue(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

void Show_Voltage_State()
{
	OLED_ShowString(4, 1, "V4:0.00V V5:0.00V");

	int i=0;
	for(i=0;i<2;i++)
	{
		Voltage[i] = (float)AD_Value[i] * 3.366 / 4095 ;
		OLED_ShowNum(4, 4+9*i, Voltage[i], 1);
		OLED_ShowNum(4, 6+9*i, (uint16_t)(Voltage[i] * 100) % 100, 2);
	}
	
}

void JoyStick_Init(void)
{
	//GPIOA PIN3/4 开启ADC读取摇杆键值
	AD_Init();
	
	//GPIOB PIN12/13 开启摇杆按下中断
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11 | EXTI_Line12 | EXTI_Line13;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

}


/*********************************************************
函数功能：云台由JOYSTICK控制器控制
*********************************************************/
#include "servo_motor.h"
extern float Voltage[2];
#define X_MAX_VOLTAGE 340	//x轴电压值上限
#define Y_MAX_VOLTAGE 342	//y轴电压值上限
#define V_X_DIR -1	//方便更改旋转方向
#define V_Y_DIR -1	//方便更改旋转方向

int v_x=X_MAX_VOLTAGE/2,v_y=Y_MAX_VOLTAGE/2;		//跟随点当前坐标
int JoyStick_Control_Stop_Sign=1;
//云台水平方向旋转PID值
float v_x_Kp=0.03;
float v_x_Ki=0;
float v_x_Kd=0.001;
//云台竖直方向旋转PID值
float v_y_Kp=0.03;
float v_y_Ki=0;
float v_y_Kd=0.001;

void JoyStick_Control(void)
{
	
	int i,diff_x,diff_y;
	while(JoyStick_Control_Stop_Sign)
	{
		for(i=0;i<2;i++)
		{
			Voltage[i] = (float)AD_Value[i] * 3.366 / 4095 ;
		}
		v_x=100*Voltage[0];v_y=100*Voltage[1];
		diff_x=Pid_Control(v_x_Kp,v_x_Ki,v_x_Kd,X_MAX_VOLTAGE/2,v_x,PID_INCREASE);
		diff_y=Pid_Control(v_y_Kp,v_y_Ki,v_y_Kd,Y_MAX_VOLTAGE/2,v_y,PID_INCREASE);
		Spinnig_Level(V_X_DIR*diff_x);
		Spinnig_Vert(V_Y_DIR*diff_y);
		Timer_delay_ms(10);
	}
}
