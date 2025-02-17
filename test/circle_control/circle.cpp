#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;

//begin::uart部分***************************************************************************************************
#include "uart.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>       // Used for UART
#include <sys/fcntl.h>    // Used for UART
#include <termios.h>      // Used for UART
#include <string.h>

using namespace std;

Uart :: Uart (){
  int ii, jj, kk;
  struct termios  port_options;   // Create the structure

  tcgetattr(fid, &port_options);	// Get the current attributes of the Serial port
  fid = open(uart_target, O_RDWR | O_NOCTTY );
  tcflush(fid, TCIFLUSH);
  tcflush(fid, TCIOFLUSH);
  if (fid == -1)
  {
    printf("**Error - Unable to open UART**.  \n=>Ensure it is not in use by another application\n=>Ensure proper privilages are granted to accsess /dev/.. by run as a sudo\n");
  }
  port_options.c_cflag &= ~PARENB;            // Disables the Parity Enable bit(PARENB),So No Parity
  port_options.c_cflag &= ~CSTOPB;            // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit
  port_options.c_cflag &= ~CSIZE;	            // Clears the mask for setting the data size
  port_options.c_cflag |=  CS8;               // Set the data bits = 8
  port_options.c_cflag &= ~CRTSCTS;           // No Hardware flow Control
  port_options.c_cflag |=  CREAD | CLOCAL;                  // Enable receiver,Ignore Modem Control lines
  port_options.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable XON/XOFF flow control both input & output
  port_options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode
  port_options.c_oflag &= ~OPOST;                           // No Output Processing

  port_options.c_lflag = 0;               //  enable raw input instead of canonical,

  port_options.c_cc[VMIN]  = VMINX;       // Read at least 1 character
  port_options.c_cc[VTIME] = 0;           // Wait indefinetly

  cfsetispeed(&port_options,BAUDRATE);    // Set Read  Speed
  cfsetospeed(&port_options,BAUDRATE);    // Set Write Speed

  // Set the attributes to the termios structure
  int att = tcsetattr(fid, TCSANOW, &port_options);

  if (att != 0 )
  {
    printf("\nERROR in Setting port attributes");
  }
  else
  {
    printf("\nSERIAL Port Good to Go.\n");
  }

  // Flush Buffers
  tcflush(fid, TCIFLUSH);
  tcflush(fid, TCIOFLUSH);
}

void Uart :: sendUart(char *msg){
  //--------------------------------------------------------------
  // TRANSMITTING BYTES
  //--------------------------------------------------------------
  char tx_buffer[256]={0};

  for (int i = 0; msg[i]!='\0'; i++) {
    tx_buffer[i] = msg[i];
  }
  printf("%s\n",tx_buffer);

  if (fid != -1)
  {
    int count = write(fid, &tx_buffer[0], strlen((const char*)tx_buffer));		//Filestream, bytes to write, number of bytes to write

    printf("Count = %d\n", count);

    if (count < 0)  printf("UART TX error\n");
  }

}

void Uart :: readUart(){

  //--------------------------------------------------------------
  // RECEIVING BYTES - AND BUILD MESSAGE RECEIVED
  //--------------------------------------------------------------
  unsigned char rx_buffer[VMINX];
  bool          pickup = true;
  int ii;
  int           rx_length;
  int           nread = 0;

  //更新缓存
  tcflush(fid, TCIOFLUSH);
  //清空接收区
  for (ii=0; ii<NSERIAL_CHAR; ii++)  serial_message[ii]='\0';

  while (pickup && fid != -1)
  {
    rx_length = read(fid, (void*)rx_buffer, VMINX);   // Filestream, buffer to store in, number of bytes to read (max)

    if (rx_length>=0)
    {
      if (nread<=NSERIAL_CHAR-1){
        serial_message[nread] = rx_buffer[0];   // Build message 1 character at a time
      }

      if (rx_buffer[0]=='$')   pickup=false;    // # symbol is terminator 自定协议规定以$结尾
    }

    nread++;
  }

}

int Uart :: fputc(int ch, FILE *f)
{
	write(fid, &ch, 1);
	return ch;
}

void Uart :: Uart_Printf(char *format, ...)
{
	char String[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	sendUart(String);
}

void Uart :: closeUart(){
  //-------------------------------------------
  //  CLOSE THE SERIAL PORT
  //-------------------------------------------
  close(fid);
}
//end::uart部分***************************************************************************************************

const char* windowname="win";
int max_r=100;
int min_r=60;
int min_d=80;
int t_hold=25;
int param1=100;
int param2=20;
const int t_max=255;
const int r_max=1000;
const int d_max=100;
const int p1_max=200;
const int p2_max=200;


int main()
{
  VideoCapture capture(0);
  Mat image;
	Mat matCanny;
  Mat BinImg;
  Mat matDst;
  Uart u;

  void on_Trackbar_1(int, void*);
  void on_Trackbar_2(int, void*);
  void on_Trackbar_3(int, void*);
  void on_Trackbar_4(int, void*);
  void on_Trackbar_5(int, void*);
  void on_Trackbar_6(int, void*);
  namedWindow(windowname,0);
  setWindowProperty(windowname, WND_PROP_ASPECT_RATIO , WINDOW_FREERATIO);
  resizeWindow(windowname, 400, 300);
  moveWindow(windowname, 0, 0);
  createTrackbar("t_hold",windowname, &t_hold, t_max, on_Trackbar_3);
  createTrackbar("max_r",windowname, &max_r, r_max, on_Trackbar_1);
  createTrackbar("min_r",windowname, &min_r, r_max, on_Trackbar_2);
  createTrackbar("min_d",windowname, &min_d, d_max, on_Trackbar_6);
  createTrackbar("p_1",windowname, &param1, p1_max, on_Trackbar_4);
  createTrackbar("p_2",windowname, &param2, p2_max, on_Trackbar_5);

	while (capture.isOpened())
	{
		capture >> image;
		if (image.empty())break;
    cvtColor(image, matDst, COLOR_BGR2GRAY);
		threshold(matDst, BinImg, t_hold, 255, THRESH_BINARY_INV|THRESH_OTSU);
    Canny(BinImg, matCanny, 100, 300, 3, false);		//canny算子
        
		std::vector<Vec3f> circles;
		HoughCircles(matCanny, circles, HOUGH_GRADIENT, 1, min_d, param1, param2, min_r, max_r);
		//在原图中画出圆心和圆  
		for (size_t i = 0; i < circles.size(); i++) {
			//提取出圆心坐标  
			Point center(round(circles[i][0]), round(circles[i][1]));
			//提取出圆半径  
			int radius = round(circles[i][2]);
			//圆心  
			circle(image, center, 3, Scalar(255,0,0), -1, 4, 0);
			//圆  
			circle(image, center, radius, Scalar(255,0,0), 3, 4, 0);
		}

    printf("x:%d\ny:%d\n",(int)round(circles[0][0]),(int)round(circles[0][1]));
    u.Uart_Printf("#x%dy%d$",(int)round(circles[0][0]),(int)round(circles[0][1]));

    imshow("matCanny", matCanny);
    imshow("BinImg", BinImg);
		imshow(windowname, image);
		if (waitKey(1) == 27)break;
	}

  u.closeUart();
  return 0;

}


void on_Trackbar_1(int, void*)
{
    ;
}


void on_Trackbar_2(int, void*)
{
    ;
}

void on_Trackbar_3(int, void*)
{
    ;
}

void on_Trackbar_4(int, void*)
{
    ;
}

void on_Trackbar_5(int, void*)
{
    ;
}

void on_Trackbar_6(int, void*)
{
    ;
}
