#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/videoio.hpp>
#include "uart.h"
using namespace std;
using namespace cv;

int capture_width = 1640;
int capture_height = 1232;
int display_width = 1640;
int display_height = 1232;
int framerate = 30;
int flip_method = 0;

string gstreamer_pipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method)
{
   return "nvarguscamerasrc exposurecompensation=-2 saturation=2 ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main()
{

   //创建管道
   string pipeline = gstreamer_pipeline(capture_width,
       capture_height,
       display_width,
       display_height,
       framerate,
       flip_method);

   //管道与视频流绑定
   VideoCapture capture(pipeline, CAP_GSTREAMER);


   Mat image;

   //逐帧显示
   while (true)
   {
       capture.read(image);
       imshow("CSI Camera", image);

       if (waitKey(10) == 27)break;
   }

   capture.release();
   destroyAllWindows();
}