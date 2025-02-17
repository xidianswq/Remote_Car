#include "atmg.h"

void errorLog(int num)
{
	while (1)
	{
	  	BlueTooth_TransmitPrintf("ERROR%d\r\n",num);
	}
}

void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		BlueTooth_TransmitPrintf("**************\r\n");
		BlueTooth_TransmitPrintf("%s",Save_Data.GPS_Buffer);
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W
						//case 7:memcpy(Save_Data.Speed, subString, subStringNext - subString);break;	//获取速度
						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}
		}
	}
}


void printGpsBuffer()
{
	float Lat;
	float Lon;
	int iLat;
	int iLon;
	if (Save_Data.isParseData)
	{
		int i;
		Save_Data.isParseData = false;
		if(Save_Data.isUsefull)
		{
			BlueTooth_TransmitPrintf("Save_Data.latitude = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.latitude);
			BlueTooth_TransmitPrintf("\r\n");


			BlueTooth_TransmitPrintf("Save_Data.N_S = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.N_S);
			BlueTooth_TransmitPrintf("\r\n");

			BlueTooth_TransmitPrintf("Save_Data.longitude = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.longitude);
			BlueTooth_TransmitPrintf("\r\n");

			BlueTooth_TransmitPrintf("Save_Data.E_W = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.E_W);
			BlueTooth_TransmitPrintf("\r\n");
			
			
			float tmp=0;
			int j=0;
			for(i=0;Save_Data.latitude[i]!= 0 ; i++)
			{
				if(Save_Data.latitude[i]=='.')
				{
					continue;
				}
				if(i<=1)
				{
					//取个位和十位；
					Lat=(Save_Data.latitude[0]-48)*10+(Save_Data.latitude[1]-48);
				}
				else
				{
					tmp+=(Save_Data.latitude[i]-48);
					tmp*=10;
				}
			}
			for(j=0;j<=5;j++)
			{
				tmp/=10;
			}
			Lat+=tmp/60;
			iLat=0;
			iLat=(int)Lat;
			Save_Data.latitude[0]=iLat/10+'0';
			Save_Data.latitude[1]=iLat%10+'0';
			Save_Data.latitude[2]='.';
			Lat-=iLat;
			for(j=3;j<10;j++)
			{
				Lat*=10;
				iLat=(int)Lat;
				Save_Data.latitude[j]=iLat+'0';
				Lat-=iLat;
			}
			tmp=0;
			
			for(i=0;Save_Data.longitude[i]!= 0 ;i++)
			{
				if(Save_Data.longitude[i]=='.')
				{
					continue;
				}
				if(i<=2)
				{
					Lon=(((Save_Data.longitude[0]-48)*10+(Save_Data.longitude[1]-48))*10)+(Save_Data.longitude[2]-48);
				}
				else
				{
					tmp+=(Save_Data.longitude[i]-48);
					tmp*=10;
				}
			}
			for(j=0;j<=5;j++)
			{
				tmp/=10;
			}
			iLon=0;
			Lon+=tmp/60;
			iLon=(int)Lon;
			Save_Data.longitude[0]=iLon/100+'0';
			Save_Data.longitude[1]=(iLon%100)/10+'0';
			Save_Data.longitude[2]=iLon%10+'0';
			Save_Data.longitude[3]='.';
			Lon-=iLon;
			for(j=4;j<11;j++)
			{
				Lon*=10;
				iLon=(int)Lon;
				Save_Data.longitude[j]=iLon+'0';
				Lon-=iLon;
			}
//			float speed;
//			for(i=0;Save_Data.Speed[i]!= 0 ;i++)
//			{
//				if(i<1)
//				{
//					speed+=(Save_Data.Speed[i]-48);
//				}
//				if(Save_Data.Speed[i]=='.')
//				{
//					continue;
//				}
//				else
//				{
//					speed+=(Save_Data.Speed[i]-48)/pow(10,i-1);
//				}
//			}
//			speed*=0.514;
//			int ispeed;
//			ispeed=(int)speed;
//			Save_Data.Speed[0]=ispeed+'0';
//			Save_Data.Speed[1]='.';
//			speed-=ispeed;
//			for(i=2;i<5;i++)
//			{
//				speed*=10;
//				ispeed=(int)speed;
//				Save_Data.Speed[i]=ispeed+'0';
//				speed-=ispeed;
//			}
			
			BlueTooth_TransmitPrintf("latitude = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.latitude);
			BlueTooth_TransmitPrintf("\r\n");

			BlueTooth_TransmitPrintf("N_S = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.N_S);
			BlueTooth_TransmitPrintf("\r\n");

			BlueTooth_TransmitPrintf("longitude = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.longitude);
			BlueTooth_TransmitPrintf("\r\n");

			BlueTooth_TransmitPrintf("E_W = ");
			BlueTooth_TransmitPrintf("%s",Save_Data.E_W);
			BlueTooth_TransmitPrintf("\r\n");
			
//			BlueTooth_TransmitPrintf("Speed = ");
//			BlueTooth_TransmitPrintf("%s",Save_Data.Speed);
//			BlueTooth_TransmitPrintf("\r\n");
			
		}

		else
		{
			BlueTooth_TransmitPrintf("GPS DATA is not usefull!\r\n");
		}
	}
}

