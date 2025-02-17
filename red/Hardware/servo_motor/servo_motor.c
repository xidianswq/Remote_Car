#include "servo_motor.h"

int Oc_L=SERVO090;		//水平方向舵机实时OC比较寄存器值
int Oc_V=SERVO090;		//竖直方向舵机实时OC比较寄存器值
int Oc_Lp[4]={750,750,750,750};
int Oc_Vp[4]={763,763,763,763};

/*********************************************************
函数功能：云台水平方向旋转
*********************************************************/
void Spinnig_Level(int diff)
{
	if(diff<0)
	{
		Oc_Lp[0]=Oc_L=(Oc_L+diff)<660?660:(Oc_L+diff);
	}
	else if(diff>0)
	{
		Oc_Lp[0]=Oc_L=(Oc_L+diff)>840?840:(Oc_L+diff);	
	}
	TIM_SetCompare1(TIM1,Oc_L);	
	int i;
	for(i=3;i>0;i--)Oc_Lp[i]=Oc_Lp[i-1];
}



/*********************************************************
函数功能：云台竖直方向旋转
*********************************************************/
void Spinnig_Vert(int diff)
{
	if(diff<0)
	{
		Oc_Vp[0]=Oc_V=(Oc_V+diff)<690?690:(Oc_V+diff);
	}
	else if(diff>0)
	{
		Oc_Vp[0]=Oc_V=(Oc_V+diff)>845?845:(Oc_V+diff);	
	}
	TIM_SetCompare4(TIM1,Oc_V);
	int i;
	for(i=3;i>0;i--)Oc_Vp[i]=Oc_Vp[i-1];
}



///*********************************************************
//函数功能：云台竖直方向设置角度
//*********************************************************/
//void Set_Angle_Vert(float angle)
//{
//	Oc_V=angle*(SERVO180-SERVOSUB)/180+SERVOADJUST;	//使用矫正值使水平
//	if(Oc_V<=SERVO000)Oc_V=SERVO000;
//	else if(Oc_V>=SERVO180)Oc_V=SERVO180;	
//	TIM_SetCompare4(TIM1,Oc_V);
//}

void Show_Pwm_Compare(void)
{
	OLED_ShowString(1,1,"L:0000 V:0000");
	OLED_ShowNum(1,3,Oc_L,4);
	OLED_ShowNum(1,3,Oc_V,4);
}


//********************************************************开环控制函数(OpenLoop--OL)********************************************************
#define OrginPos_Pwm_L 741		//正方形中心水平Level方向复位Pwm值（OC1比较寄存器）
#define OrginPos_Pwm_V 765		//正方形中心竖直Vert方向复位Pwm值（OC4比较寄存器）
#define LeftUpPos_Pwm_L 825		//正方形左上角水平Level方向复位Pwm值（OC1比较寄存器）
#define LeftUpPos_Pwm_V 696 	//正方形左上角竖直Vert方向复位Pwm值（OC4比较寄存器）
/*********************************************************
函数功能：云台控制激光点到达某一点
*********************************************************/
void Reach_Pos_OL(int Pos_Pwm_L,int Pos_Pwm_V)
{
	Oc_L=Pos_Pwm_L;Oc_V=Pos_Pwm_V;
	TIM_SetCompare1(TIM1,Oc_L);
	TIM_SetCompare4(TIM1,Oc_V);
}

/*********************************************************
函数功能：云台控制激光点复位
*********************************************************/
void Restart_Pos_OL(void)
{
	Reach_Pos_OL(OrginPos_Pwm_L,OrginPos_Pwm_V);
}

/*********************************************************
函数功能：云台控制激光点绕正方形顺时针旋转一周
*********************************************************/
int Follow_Track_Stop_Sign=1;
void Follow_Track_OL(void)
{
	Reach_Pos_OL(LeftUpPos_Pwm_L,LeftUpPos_Pwm_V);
	Timer_delay_ms(400);
	Reach_Pos_OL(2*OrginPos_Pwm_L-LeftUpPos_Pwm_L,LeftUpPos_Pwm_V);
	Timer_delay_ms(400);
	Reach_Pos_OL(2*OrginPos_Pwm_L-LeftUpPos_Pwm_L,2*OrginPos_Pwm_V-LeftUpPos_Pwm_V);
	Timer_delay_ms(400);
	Reach_Pos_OL(LeftUpPos_Pwm_L,2*OrginPos_Pwm_V-LeftUpPos_Pwm_V);
	Timer_delay_ms(400);
	Reach_Pos_OL(LeftUpPos_Pwm_L,LeftUpPos_Pwm_V);
	Timer_delay_ms(400);
}



//********************************************************闭环控制函数(CloseLoop--CL)********************************************************
#include "usart.h"
#define X_DIR -1	//方便更改旋转方向
#define Y_DIR 1	//方便更改旋转方向
int OrginPos_Pos_L=360;		//正方形中心水平Level方向坐标
int OrginPos_Pos_V=360;		//正方形中心竖直Vert方向坐标
int LeftUpPos_Pos_L=10;		//正方形左上角水平Level方向坐标
int LeftUpPos_Pos_V=10; 	//正方形左上角竖直Vert方向坐标
int RightDownPos_Pos_L=710;		
int RightDownPos_Pos_V=710;
int Vertex[4][2];	
/*********************************************************
函数功能：stm32获取重要参考点坐标
*********************************************************/
extern char  USART_RX_INFO[USART_REC_LEN];	//uart接收数据
void Get_Depend_Point_Pos(void)
{
	while(1)
	{
		if(USART_RX_INFO[0]=='a'&& USART_RX_INFO[4]=='b' && USART_RX_INFO[8]=='c' && USART_RX_INFO[12]=='d' )	//检查数据定位是否正确(上位机发送信息为：a123b456c123d123)
		{
			Vertex[0][0]=LeftUpPos_Pos_L=(USART_RX_INFO[1]-'0')*100+(USART_RX_INFO[2]-'0')*10+USART_RX_INFO[3]-'0';
			Vertex[0][1]=LeftUpPos_Pos_V=(USART_RX_INFO[5]-'0')*100+(USART_RX_INFO[6]-'0')*10+USART_RX_INFO[7]-'0';
			Vertex[2][0]=RightDownPos_Pos_L=(USART_RX_INFO[9]-'0')*100+(USART_RX_INFO[10]-'0')*10+USART_RX_INFO[11]-'0';
			Vertex[2][1]=RightDownPos_Pos_V=(USART_RX_INFO[13]-'0')*100+(USART_RX_INFO[14]-'0')*10+USART_RX_INFO[15]-'0';
			Vertex[1][0]=Vertex[2][0];
			Vertex[1][1]=Vertex[0][1];
			Vertex[3][0]=Vertex[0][0];
			Vertex[3][1]=Vertex[2][1];
			OrginPos_Pos_L=(LeftUpPos_Pos_L+RightDownPos_Pos_L)/2;
			OrginPos_Pos_V=(LeftUpPos_Pos_V+RightDownPos_Pos_V)/2;
			
			Usart_TransmitString("STM32:SUCCEED TO GET DEPENDENT POINT!\n$");
			Beep_Times(50,1,NORMAL_MODE);
			break;
		}
		else 
		{
			Usart_TransmitString("STM32:FAILD TO GET DEPENDENT POINT!\n$");
			Beep_Times(50,2,NORMAL_MODE);
		}
	}
}

/*********************************************************
函数功能：云台控制激光点到达某一点
函数参数：目标点的坐标
*********************************************************/
int x=360,y=360;		//跟随点当前坐标
int Reach_Pos_CL_Stop_Sign=1;
//云台水平方向旋转PID值
float Level_Kp=0.06;
float Level_Ki=0.02;
float Level_Kd=0.01;
//云台竖直方向旋转PID值
float Vert_Kp=0.06;
float Vert_Ki=0.02;
float Vert_Kd=0.01;
void Reach_Pos_CL(int Target_X,int Target_Y,int Reach_Pos_CL_MODE)
{
	int Sign(int num);
	void Get_Point_Pos(void);
	int near(int Target_X,int Target_Y);
	
	int diff_x,diff_y;
	while(Reach_Pos_CL_Stop_Sign)
	{
		Timer_delay_ms(30);
		Get_Point_Pos();
		if(near(Target_X,Target_Y)<=6)
		{
			Beep_Times(10,1,NORMAL_MODE);
			break;
		}
		if(Reach_Pos_CL_MODE==PID_MODE && near(Target_X,Target_Y)>60)					//用pid计算舵机单位数
		{
			diff_x=Pid_Control(Level_Kp,Level_Ki,Level_Kd,Target_X,x,PID_REALIZE);
			diff_y=Pid_Control(Vert_Kp,Vert_Ki,Vert_Kd,Target_Y,y,PID_REALIZE);
		}
		else if(Reach_Pos_CL_MODE==MINMIZE_MODE)		//以舵机最小分辨率为单位
		{
			diff_x=-Sign(x-Target_X);
			diff_y=-Sign(y-Target_Y);
		}
		else if(Reach_Pos_CL_MODE==PID_MODE && near(Target_X,Target_Y)<=60)					//用pid计算舵机单位数
		{
			diff_x=-Sign(x-Target_X);
			diff_y=-Sign(y-Target_Y);
			Timer_delay_ms(30);
		}
		Spinnig_Level(X_DIR*diff_x);
		Spinnig_Vert(Y_DIR*diff_y);
		Timer_delay_ms(20);
	}
}

int Sign(int num)
{
	if(num>5)return 1;
	else if(num<-5)return -1;
	else return 0;
}

int my_abs(int a,int b)
{
	return a-b>0?a-b:b-a;
}

int near(int Target_X,int Target_Y)
{
	return my_abs(Target_X,x)+my_abs(Target_Y,y);
}

/*********************************************************
函数功能：stm32获取当前激光坐标
*********************************************************/
void Get_Point_Pos(void)
{
	if(USART_RX_INFO[0]=='x')				//检查数据定位是否正确(上位机发送信息为：x123y456)
	{
		x=(USART_RX_INFO[1]-'0')*100+(USART_RX_INFO[2]-'0')*10+USART_RX_INFO[3]-'0';
	}
	if(USART_RX_INFO[4]=='y')				//检查数据定位是否正确(上位机发送信息为：x123y456)
	{
		y=(USART_RX_INFO[5]-'0')*100+(USART_RX_INFO[6]-'0')*10+USART_RX_INFO[7]-'0';
	}
}

/*********************************************************
函数功能：云台控制激光复位
*********************************************************/
void Restart_Pos_CL(int Reach_Pos_CL_MODE)
{
	Reach_Pos_CL(OrginPos_Pos_L,OrginPos_Pos_V,Reach_Pos_CL_MODE);
}

/*********************************************************
函数功能：云台控制激光在四边形边上顺时针旋转
*********************************************************/
void Follow_Track_CL(int Vertex[4][2],int divide_num,int Reach_Pos_CL_MODE)
{
	int i,j;
	float sub_x,sub_y;
	
	Reach_Pos_CL(Vertex[0][0],Vertex[0][1],Reach_Pos_CL_MODE);
	
	for(i=0;i<4;i++)
	{
		sub_x=(Vertex[(i+1)%4][0]-Vertex[i][0])/divide_num;	//下一个顶点与当前顶点横坐标之差
		sub_y=(Vertex[(i+1)%4][1]-Vertex[i][1])/divide_num;	//下一个顶点与当前顶点纵坐标之差
		for(j=0;j<divide_num;j++)
		{
			Reach_Pos_CL(Vertex[i][0]+j*sub_x,Vertex[i][1]+j*sub_y,Reach_Pos_CL_MODE);
			Timer_delay_ms(20);
		}
	}
	Reach_Pos_CL(Vertex[0][0],Vertex[0][1],Reach_Pos_CL_MODE);	//回到起点
	Beep_Times(500,1,NORMAL_MODE);
}

/*********************************************************
函数功能：stm32获取A4纸坐标
*********************************************************/
int Vertex_A4[4][2];
void Get_A4_Point_Pos(void)
{
//	while(1)
//	{
//		if(USART_RX_INFO[0]=='a'&& USART_RX_INFO[4]=='b' && USART_RX_INFO[8]=='c' && USART_RX_INFO[12]=='d'&&USART_RX_INFO[16]=='e'&& USART_RX_INFO[20]=='f' && USART_RX_INFO[24]=='g' && USART_RX_INFO[28]=='h' )	//检查数据定位是否正确(上位机发送信息为：a123b456c123d123)
//		{
//			Vertex_A4[0][0]=(USART_RX_INFO[1]-'0')*100+(USART_RX_INFO[2]-'0')*10+USART_RX_INFO[3]-'0';
//			Vertex_A4[0][1]=(USART_RX_INFO[5]-'0')*100+(USART_RX_INFO[6]-'0')*10+USART_RX_INFO[7]-'0';
//			Vertex_A4[1][0]=(USART_RX_INFO[9]-'0')*100+(USART_RX_INFO[10]-'0')*10+USART_RX_INFO[11]-'0';
//			Vertex_A4[1][1]=(USART_RX_INFO[13]-'0')*100+(USART_RX_INFO[14]-'0')*10+USART_RX_INFO[15]-'0';
//			Vertex_A4[2][0]=(USART_RX_INFO[17]-'0')*100+(USART_RX_INFO[18]-'0')*10+USART_RX_INFO[19]-'0';
//			Vertex_A4[2][1]=(USART_RX_INFO[21]-'0')*100+(USART_RX_INFO[22]-'0')*10+USART_RX_INFO[23]-'0';
//			Vertex_A4[3][0]=(USART_RX_INFO[25]-'0')*100+(USART_RX_INFO[26]-'0')*10+USART_RX_INFO[27]-'0';
//			Vertex_A4[3][1]=(USART_RX_INFO[29]-'0')*100+(USART_RX_INFO[30]-'0')*10+USART_RX_INFO[31]-'0';
//			
//			Usart_TransmitString("STM32:SUCCEED TO GET DEPENDENT POINT!\n$");
//			Beep_Times(50,5,NORMAL_MODE);
//			break;
//		}
//		else 
//		{
//			Usart_TransmitString("STM32:FAILD TO GET DEPENDENT POINT!\n$");
//		}
//	}
	
	int i,j;
	while(1)
	{
		for(i=0;i<8;i++)
		{
			if(USART_RX_INFO[4*i]=='k'+i)continue;
			else break;
		}
		if(i==8)
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<2;j++)Vertex_A4[i][j]=(USART_RX_INFO[4*(2*i+j)+1]-'0')*100+(USART_RX_INFO[4*(2*i+j)+2]-'0')*10+(USART_RX_INFO[4*(2*i+j)+3]-'0');
			}
			Usart_TransmitString("STM32:SUCCEED TO GET DEPENDENT POINT!\n$");
			Beep_Times(50,5,NORMAL_MODE);
			break;
		}
	}
}

/*********************************************************
函数功能：点跟随功能
*********************************************************/
int Red_Pos_X;
int Red_Pos_Y;
int Follow_Point_Stop_Sign=1;
void Follow_Point(void)
{
	void Get_Red_Point_Pos();
	while(Follow_Point_Stop_Sign)
	{
		Get_Point_Pos();
		Get_Red_Point_Pos();
		Reach_Pos_CL(Red_Pos_X,Red_Pos_Y,PID_MODE);
	}

}

void Get_Red_Point_Pos(void)
{
	if(USART_RX_INFO[8]=='x')				//检查数据定位是否正确(上位机发送信息为：x123y456)
	{
		Red_Pos_X=(USART_RX_INFO[9]-'0')*100+(USART_RX_INFO[10]-'0')*10+USART_RX_INFO[11]-'0';
	}
	if(USART_RX_INFO[12]=='y')				//检查数据定位是否正确(上位机发送信息为：x123y456)
	{
		Red_Pos_Y=(USART_RX_INFO[13]-'0')*100+(USART_RX_INFO[14]-'0')*10+USART_RX_INFO[15]-'0';
	}
}


//********************************************************混合控制函数(CloseLoop--CL)********************************************************
void Reach_LeftUPPos(void)
{
	Reach_Pos_OL(LeftUpPos_Pwm_L,LeftUpPos_Pwm_V);
	Reach_Pos_CL(LeftUpPos_Pos_L,LeftUpPos_Pos_V,MINMIZE_MODE);
}

void Reach_OrginPos(void)
{
	Reach_Pos_OL(OrginPos_Pwm_L,OrginPos_Pwm_V);
	Reach_Pos_CL(OrginPos_Pos_L,OrginPos_Pos_V,MINMIZE_MODE);
}










//********************************************************高级控制函数(CloseLoop--CL)********************************************************
int Vertex_Peak_Pos[4][2];
int Center_Pos[2];
Pwm Center_Pwm;
Pwm Peak_Pwm[4];
Pwm A4_Pwm[4]; 
//获取重要点坐标
void Get_Point_5(void)
{
	int i,j;
	while(1)
	{
		for(i=0;i<8;i++)
		{
			if(USART_RX_INFO[4*i]=='a'+i)continue;
			else break;
		}
		if(i==8)
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<2;j++)Vertex_Peak_Pos[i][j]=(USART_RX_INFO[4*(2*i+j)+1]-'0')*100+(USART_RX_INFO[4*(2*i+j)+2]-'0')*10+(USART_RX_INFO[4*(2*i+j)+3]-'0');
			}
			break;
		}
	}
	
	while(!(USART_RX_INFO[0]=='i'&&USART_RX_INFO[4]=='j'));
	
	Center_Pos[0]=(USART_RX_INFO[1]-'0')*100+(USART_RX_INFO[2]-'0')*10+USART_RX_INFO[3]-'0';
	Center_Pos[1]=(USART_RX_INFO[5]-'0')*100+(USART_RX_INFO[6]-'0')*10+USART_RX_INFO[7]-'0';
	Beep_Times(50,5,NORMAL_MODE);
	
	
	
//	int i,j,sign=0;
//	for(i=0;i<10;i++)
//	{
//		if(USART_RX_INFO[4*i]=='a'+i)continue;
//		else break;
//	}
//	if(i==10)
//	{
//		for(i=0;i<4;i++)
//		{
//			for(j=0;j<2;j++)Vertex_Peak_Pos[i][j]=(USART_RX_INFO[4*(2*i+j)+1]-'0')*100+(USART_RX_INFO[4*(2*i+j)+2]-'0')*10+(USART_RX_INFO[4*(2*i+j)+3]-'0');
//		}
//		Center_Pos[0]=(USART_RX_INFO[33]-'0')*100+(USART_RX_INFO[34]-'0')*10+USART_RX_INFO[35]-'0';
//		Center_Pos[1]=(USART_RX_INFO[37]-'0')*100+(USART_RX_INFO[38]-'0')*10+USART_RX_INFO[39]-'0';
//		return 1;
//	}
//	
//	return 0;
}

void Axes_Init(void)
{
	Get_Point_5();
	
	int i;
	for(i=0;i<4;i++)Get_Pwm(Vertex_Peak_Pos[i][0],Vertex_Peak_Pos[i][1],&Peak_Pwm[i],1);
	Get_Pwm(Center_Pos[0],Center_Pos[1],&Center_Pwm,1);
	Beep_Times(50,5,NORMAL_MODE);
}

int sum_num(int *num,int n)
{
	int i,sum;
	for(i=sum=0;i<n;i++)sum+=num[i];
	return sum;
}	

//获取目标点pwm值
void Get_Pwm(int px,int py,Pwm *target_pwm,int n)
{
	Reach_Pos_CL(px,py,PID_MODE);
	target_pwm->level=sum_num(Oc_Lp,n)/n;
	target_pwm->vert=sum_num(Oc_Vp,n)/n;
}

void Pwm_Track(Pwm Vertex_Pwm[4],int divide_num)
{
	int i,j;
	float sub_l,sub_v;
	
	for(i=0;i<4;i++)
	{
		sub_l=(Vertex_Pwm[(i+1)%4].level-Vertex_Pwm[i].level)/divide_num;	//下一个顶点与当前顶点pwm之差
		sub_v=(Vertex_Pwm[(i+1)%4].vert-Vertex_Pwm[i].vert)/divide_num;	//下一个顶点与当前顶点纵坐标之差
		for(j=0;j<divide_num;j++)
		{
			Reach_Pos_OL(Vertex_Pwm[i].level+j*sub_l,Vertex_Pwm[i].vert+j*sub_v);
			Timer_delay_ms(200);
		}
		Reach_Pos_OL(Vertex_Pwm[(i+1)%4].level,Vertex_Pwm[(i+1)%4].vert);
		Timer_delay_ms(300);
	}
	
	Beep_Times(50,5,NORMAL_MODE);
}

//巡线
void Follow_Track(int Vertex[4][2],int divide_num)
{
	int i,j;
	float sub_l,sub_v;
	Pwm Vertex_Pwm[4];
	for(i=0;i<4;i++)Get_Pwm(Vertex[i][0],Vertex[i][1],&Vertex_Pwm[i],1);
	
	for(i=0;i<4;i++)
	{
		sub_l=(Vertex_Pwm[(i+1)%4].level-Vertex_Pwm[i].level);	//下一个顶点与当前顶点pwm之差
		sub_v=(Vertex_Pwm[(i+1)%4].vert-Vertex_Pwm[i].vert);	//下一个顶点与当前顶点纵坐标之差
		for(j=0;j<divide_num;j++)
		{
			Reach_Pos_OL(Vertex_Pwm[i].level+j*sub_l/divide_num,Vertex_Pwm[i].vert+j*sub_v/divide_num);
			Timer_delay_ms(200);
		}
		Reach_Pos_OL(Vertex_Pwm[(i+1)%4].level,Vertex_Pwm[(i+1)%4].vert);
		Timer_delay_ms(300);
	}
	
	Beep_Times(50,5,NORMAL_MODE);
}


