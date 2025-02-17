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
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

string gstreamer_pipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method)
{
   return "nvarguscamerasrc exposurecompensation=0 ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

const char* windowname = "win";
int r_hold1 = 180;
int r_hold2 = 255;
int r_hold3 = 255;

int g_hold1 = 75;
int g_hold2 = 255;
int g_hold3 = 255;


const int t_max = 255;

int myabs(int a)
{
   return a > 0 ? a : -a;
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
   VideoCapture capture(pipeline, CAP_GSTREAMER);
   Mat img,image;

   Point color_recognite(Mat image, Scalar Low, Scalar High);

   void on_Trackbar_1(int, void*);
   void on_Trackbar_2(int, void*);
   void on_Trackbar_3(int, void*);
   void on_Trackbar_4(int, void*);
   void on_Trackbar_5(int, void*);
   void on_Trackbar_6(int, void*);
   namedWindow("red", 0);
   setWindowProperty("red", WND_PROP_ASPECT_RATIO, WINDOW_FREERATIO);
   resizeWindow("red", 400, 400);
   moveWindow("red", 0, 0);

   namedWindow("green", 0);
   setWindowProperty("green", WND_PROP_ASPECT_RATIO, WINDOW_FREERATIO);
   resizeWindow("green", 400, 400);
   moveWindow("green", 0, 0);

   createTrackbar("r_hold1", "red", &r_hold1, t_max, on_Trackbar_1);
   createTrackbar("r_hold2", "red", &r_hold2, t_max, on_Trackbar_2);
   createTrackbar("r_hold3", "red", &r_hold3, t_max, on_Trackbar_3);
   createTrackbar("g_hold1", "green", &g_hold1, t_max, on_Trackbar_4);
   createTrackbar("g_hold2", "green", &g_hold2, t_max, on_Trackbar_5);
   createTrackbar("g_hold3", "green", &g_hold3, t_max, on_Trackbar_6);

   int sleep_times = 0, Find = 0, l_x = 0, l_y = 0, r_x = 0, r_y = 0;

   while (capture.isOpened())
   {
       sleep_times++;  //使摄像头画面稳定

       capture.read(img);
       if (img.empty())break;
       image=img.colRange(280,1000);
       if (waitKey(1) == 27)break;

       if (sleep_times >= 12)
       {
            // Mat HsvImg;
             Mat ImgRed,ImgGreen;
            // cvtColor(image, HsvImg, COLOR_BGR2HSV);
            // inRange(HsvImg, Scalar(r_hold1, r_hold2, r_hold3), Scalar(180, 255, 255), ImgRed);
            // inRange(HsvImg, Scalar(g_hold1, g_hold2, g_hold3), Scalar(75, 255, 255), ImgGreen);


            Mat HsvImg;
            cvtColor(image, HsvImg, COLOR_BGR2HSV);
            Point centre_r = color_recognite(image, Scalar(127, 66, 21), Scalar(210, 255, 255));
            Point centre_g = color_recognite(image, Scalar(30, 83, 24), Scalar(75, 255, 255));
            printf("r:%d %d\n",centre_r.x,centre_r.y);
            printf("g:%d %d\n",centre_g.x,centre_g.y);
            imshow("img",image);
       }
        
   }



   capture.release();
   destroyAllWindows();
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

Point color_recognite(Mat image, Scalar Low, Scalar High)
{

    vector<vector<Point>> g_vContours;
    vector<Vec4i> g_vHierarchy;
    vector<Mat> hsvSplit;
    double maxarea = 0;
    int maxAreaIdx = 0;
    Mat g_grayImage, hsv, g_cannyMat_output;

    cvtColor(image, hsv, COLOR_BGR2HSV);
    split(hsv, hsvSplit);
    equalizeHist(hsvSplit[2], hsvSplit[2]);
    merge(hsvSplit, hsv);
    inRange(hsv, Low, High, g_grayImage);//二值化识别颜色

    //开操作 (去除一些噪点)
    Mat element = getStructuringElement(MORPH_RECT, Size(8, 8));
    morphologyEx(g_grayImage, g_grayImage, MORPH_OPEN, element);

    //闭操作 (连接一些连通域)
    morphologyEx(g_grayImage, g_grayImage, MORPH_CLOSE, element);
    //  Canny(g_grayImage, g_cannyMat_output, 80, 80 * 2, 3);


      // 寻找轮廓
    findContours(g_grayImage, g_vContours, g_vHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //假设contours是用findContours函数所得出的边缘点集
    RotatedRect box;
    Point centre;

    if (g_vContours.size() != 0)
    {
        for (int index = 0; index < g_vContours.size(); index++)
        {
            double tmparea = fabs(contourArea(g_vContours[index]));
            if (tmparea > maxarea)
            {
                maxarea = tmparea;
                maxAreaIdx = index;//记录最大轮廓的索引号
            }
        }

        box = minAreaRect(g_vContours[maxAreaIdx]);
        rectangle(image, box.boundingRect(), Scalar(0, 0, 255), 2);
        centre = box.center;
    }
    return centre;
}