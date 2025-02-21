#include "pca9685.h"

int Off_L=SERVO090;
int Off_V=SERVO090;

/*********************************************************
函数功能：等待I2C返回事件
*********************************************************/
void PCA9685_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
}

/*********************************************************
函数功能：从PCA9685的一个地址写数据
*********************************************************/
void PCA9685_WriteReg(uint8_t RegAddress, uint8_t Data)
{	
	I2C_GenerateSTART(I2C1, ENABLE);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C1, PCA9685_ADDRESS, I2C_Direction_Transmitter);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C1, RegAddress);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	I2C_SendData(I2C1, Data);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

/*********************************************************
函数功能：从PCA9685的一个地址读数据
*********************************************************/
uint8_t PCA9685_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C1, ENABLE);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C1, PCA9685_ADDRESS, I2C_Direction_Transmitter);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C1, RegAddress);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C1, PCA9685_ADDRESS, I2C_Direction_Receiver);
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	PCA9685_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED);
	Data = I2C_ReceiveData(I2C1);
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	return Data;
}

/*********************************************************
函数功能：PCA9685频率修改
入口参数：freq-输出PWM频率
*********************************************************/
void PCA9685_SetPwmFreq(float freq) 
{
	int prescale,oldmode,newmode;
	float prescaleval;
	
	freq *= 1;   								//纠正频率设置中的过冲，进行校准								
	prescaleval = 25000000;							//根据公式计算prescale的值
	prescaleval /= 4096;							//prescaleval=round(osc_cloc/4096/freq)-1;
	prescaleval /= freq;
	prescaleval -= 1;
	prescale = floor(prescaleval + 0.5);		
                
	oldmode = PCA9685_ReadReg(PCA9685_MODE1);		//获得MODE1寄存器值
	newmode = (oldmode&0x7F) | 0x10; 			  	//SLEEP位 置1
	PCA9685_WriteReg(PCA9685_MODE1, newmode);  	  	//进入SLEEP模式
	PCA9685_WriteReg(PCA9685_PRESCALE, prescale);    	//设置频率
	PCA9685_WriteReg(PCA9685_MODE1, oldmode);		  	//退出SLEEP模式
	delay_us(5000);
	PCA9685_WriteReg(PCA9685_MODE1, oldmode | 0xa1);//RESTART、AI、ALLCALL三个位 置1
}

/*********************************************************
函数功能：改变通道PWM占空比
输入参数：num-使用的通道0~15
		  on-高电平开始时计数器ACK值
		  off-高电平结束时计数器ACK值
*********************************************************/
void PCA9685_SetPwm(int num, int on, int off) 
{
	PCA9685_WriteReg(LED0_ON_L+4*num,on&0xff);		//LED0_ON_L保存on低8位
	PCA9685_WriteReg(LED0_ON_H+4*num,(on>>8)&0xff);	//LED0_ON_H保存on高4位
	PCA9685_WriteReg(LED0_OFF_L+4*num,off&0xff);
	PCA9685_WriteReg(LED0_OFF_H+4*num,(off>>8)&0xff);
 }

/*********************************************************
函数功能：初始化PCA9685
*********************************************************/
void PCA9685_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1, &I2C_InitStructure);
	
	I2C_Cmd(I2C1, ENABLE);
	
	PCA9685_WriteReg(PCA9685_MODE1,0x00);
	PCA9685_SetPwmFreq(50);
	int i=0;
	for(i=0;i<16;i++)
	{
		PCA9685_SetPwm(i,0,SERVO090);
	}
	Set_Angle_Vert(0);
}

/*********************************************************
函数功能：云台水平方向旋转
*********************************************************/
void Spinnig_Level(int diff)
{
	if(Off_L+diff<=SERVO000+SERVO_L_RANGE)
	{
		Off_L=SERVO000+SERVO_L_RANGE;
	}
	else if(Off_L+diff>=SERVO180-SERVO_L_RANGE)
	{
		Off_L=SERVO180-SERVO_L_RANGE;	
	}
	else
	{
		Off_L=Off_L+diff;
	}	
	PCA9685_SetPwm(0,0,Off_L);	
}

/*********************************************************
函数功能：云台竖直方向旋转
*********************************************************/
void Spinnig_Vert(int diff)
{
	if(diff<0)
	{
		Off_V=(Off_V+diff)<(SERVO000+SERVO_V_RANGE)?(SERVO000+SERVO_V_RANGE):(Off_V+diff);
	}
	else if(diff>0)
	{
		Off_V=(Off_V+diff)>(SERVO180-SERVO_V_RANGE)?(SERVO180-SERVO_V_RANGE):(Off_V+diff);	
	}
	PCA9685_SetPwm(1,0,Off_V);
}

/*********************************************************
函数功能：云台竖直方向设置角度
*********************************************************/
void Set_Angle_Vert(float angle)
{
	Off_V=angle*(SERVO180-SERVOSUB)/180+SERVOADJUST;	//使用矫正值使水平
	if(Off_V<=SERVO000)Off_V=SERVO000;
	else if(Off_V>=SERVO180)Off_V=SERVO180;	
	PCA9685_SetPwm(1,0,Off_V);
}

/*********************************************************
函数功能：云台跟随中心点
*********************************************************/
#include "usart.h"
#define LENGTH 1640	//坐标系X上限
#define HEIGHT 1232	//坐标系Y上限
#define X_DIR 1	//方便更改旋转方向
#define Y_DIR -1	//方便更改旋转方向

int Follow_Point_Stop_Sign=1;
int x=LENGTH/2,y=HEIGHT/2;		//跟随点当前坐标
//云台水平方向旋转PID值
float Level_Kp=0.03;
float Level_Ki=0.00;
float Level_Kd=0.00;
//云台竖直方向旋转PID值
float Vert_Kp=0.03;
float Vert_Ki=0.00;
float Vert_Kd=0.00;

void Follow_Point(void)
{
	int Get_Point_Pos(void);

	int diff_x,diff_y;
	while(Follow_Point_Stop_Sign)
	{
		if(Get_Point_Pos()==2){
			diff_x=Pid_Control(Level_Kp,Level_Ki,Level_Kd,LENGTH/2,x,PID_INCREASE);
			diff_y=Pid_Control(Vert_Kp,Vert_Ki,Vert_Kd,HEIGHT/2,y,PID_INCREASE);
			Spinnig_Level(X_DIR*diff_x);
			Spinnig_Vert(Y_DIR*diff_y);
		}
		break;	//使只执行一次
	}
}

int is_num(char c){
	if(c>='0'&&c<='9')return 1;
	else return 0;
}

extern char  USART_RX_INFO[USART_REC_LEN];	//uart接收数据
int Get_Point_Pos(void)
{
	int state=0,i,j;
	for(i=0;i<2;i++){
		for(j=0;j<4;j++){
			if(is_num(USART_RX_INFO[i*5+j+1]))continue;
			else return state;
		}
	}
	if(USART_RX_INFO[0]=='x')				//检查数据定位是否正确(上位机发送信息为：x1234y5678)
	{
		x=(USART_RX_INFO[1]-'0')*1000+(USART_RX_INFO[2]-'0')*100+(USART_RX_INFO[3]-'0')*10+USART_RX_INFO[4]-'0';
		if(x>0)state++;
	}
	if(USART_RX_INFO[5]=='y')				//检查数据定位是否正确(上位机发送信息为：x1234y5678)
	{
		y=(USART_RX_INFO[6]-'0')*1000+(USART_RX_INFO[7]-'0')*100+(USART_RX_INFO[8]-'0')*10+USART_RX_INFO[9]-'0';
		if(y>0)state++;
	}
	CLR_Buf();
	return state;							//返回接受成功数
}

/*********************************************************
函数功能：云台脉宽计数值设置为nano传递值
*********************************************************/
int PWM_x=SERVO090,PWM_y=SERVO090;
void Nano_SetPWM(void){
	int Get_Nano_PWM(void);

	if(Get_Nano_PWM()==2){
		PCA9685_SetPwm(0,0,PWM_x);
		PCA9685_SetPwm(1,0,PWM_y);
	}
	
}

int Get_Nano_PWM(void)
{
	int state=0,i,j;
	for(i=0;i<2;i++){
		for(j=0;j<3;j++){
			if(is_num(USART_RX_INFO[i*4+j+1]))continue;
			else return state;
		}
	}
	if(USART_RX_INFO[0]=='x')				//检查数据定位是否正确(上位机发送信息为：x123y456)
	{
		x=(USART_RX_INFO[1]-'0')*100+(USART_RX_INFO[2]-'0')*10+(USART_RX_INFO[3]-'0');
		if(x>0)state++;
	}
	if(USART_RX_INFO[4]=='y')				//检查数据定位是否正确(上位机发送信息为：x123y456)
	{
		y=(USART_RX_INFO[5]-'0')*100+(USART_RX_INFO[6]-'0')*10+(USART_RX_INFO[7]-'0');
		if(y>0)state++;
	}
	CLR_Buf();
	return state;							//返回接受成功数
}

/*********************************************************
函数功能：云台由JOYSTICK控制器控制
*********************************************************/
#include "AD.h"
extern float Voltage[2];
#define X_MAX_VOLTAGE 330	//x轴电压值上限
#define Y_MAX_VOLTAGE 320	//y轴电压值上限
#define V_X_DIR -1	//方便更改旋转方向
#define V_Y_DIR 1	//方便更改旋转方向

int v_x=X_MAX_VOLTAGE/2,v_y=Y_MAX_VOLTAGE/2;		//跟随点当前坐标
//云台水平方向旋转PID值
float v_x_Kp=0.01;
float v_x_Ki=0;
float v_x_Kd=0.003;
//云台竖直方向旋转PID值
float v_y_Kp=0.01;
float v_y_Ki=0;
float v_y_Kd=0.003;

void JoyStick_Control(void)
{
	int JoyStick_Control_Stop_Sign=1;
	int diff_x,diff_y;
	while(JoyStick_Control_Stop_Sign)
	{
		v_x=100*Voltage[0];v_y=100*Voltage[1];
		diff_x=Pid_Control(v_x_Kp,v_x_Ki,v_x_Kd,X_MAX_VOLTAGE/2,v_x,PID_INCREASE);
		diff_y=Pid_Control(v_y_Kp,v_y_Ki,v_y_Kd,Y_MAX_VOLTAGE/2,v_y,PID_INCREASE);
		Spinnig_Level(V_X_DIR*diff_x);
		Spinnig_Vert(V_Y_DIR*diff_y);
	}
}
