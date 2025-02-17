//适用于jetson nano上的模版
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

string gstreamer_pipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method)
{
   return "nvarguscamerasrc exposurecompensation=-2 saturation=2 ee-mode=2 ee-strength=1 ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main()
{
   int capture_width = 1280;
   int capture_height = 720;
   int display_width = 1280;
   int display_height = 720;
   
   int framerate = 30;
   int flip_method = 0;

   //创建管道
   string pipeline = gstreamer_pipeline(capture_width,
       capture_height,
       display_width,
       display_height,
       framerate,
       flip_method);
   std::cout << "使用gstreamer管道: \n\t" << pipeline << "\n";

   //管道与视频流绑定
   VideoCapture cap(pipeline, CAP_GSTREAMER);

   //创建显示窗口
   namedWindow("CSI Camera", WINDOW_AUTOSIZE);
   Mat img,image;

   //逐帧显示
   while (true)
   {
       cap.read(img);
       image=img.colRange(280,1000);
       imshow("CSI Camera", image);

       if (waitKey(10) == 27)break;
   }

   cap.release();
   destroyAllWindows();
}