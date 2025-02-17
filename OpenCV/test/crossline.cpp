//适用于jetson nano上的模版
#include "uart.h"
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

#define SLEEP_TIMES 30
string gstreamer_pipeline_1(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method)
{
   return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}
string gstreamer_pipeline_2(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method)
{
   return "nvarguscamerasrc exposurecompensation=-2 saturation=2 ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}
const char* windowname = "win";
int t_hold = 105;
int param1 = 1;   //double类型的rho， 以像素为单位的距离精度。 另一种形容方式是直线搜索时的进步尺寸的单位半径。
int param2 = 180;    //double类型的theta，以弧度为单位的角度精度。另一种形容方式是直线搜索时的进步尺寸的单位角度。
int param3 = 300;   //int类型的threshold，累加平面的阈值参数，即识别某部分为图中的一条直线时它在累加平面中必须达到的值。
int min_d = 65;     //double类型的minLineLength，有默认值0，表示最低线段的长度，比这个设定参数短的线段就不能被显现出来。
int max_d = 30;    //double类型的maxLineGap，有默认值0，允许将同一行点与点之间连接起来的最大的距离。
const int t_max = 255;
const int r_max = 1000;
const int d_max = 100;
const int p1_max = 360;
const int p2_max = 200;
const int p3_max = 300;

int myabs(int a)
{
    return a > 0 ? a : -a;
}

int main()
{
    Point color_recognite(Mat image, Scalar Low, Scalar High);
    void on_Trackbar_1(int, void*);
    void on_Trackbar_2(int, void*);
    void on_Trackbar_3(int, void*);
    void on_Trackbar_4(int, void*);
    void on_Trackbar_5(int, void*);
    void on_Trackbar_6(int, void*);

    namedWindow(windowname, 0);
    setWindowProperty(windowname, WND_PROP_ASPECT_RATIO, WINDOW_FREERATIO);
    resizeWindow(windowname, 400, 400);
    moveWindow(windowname, 0, 0);
    createTrackbar("t_hold", windowname, &t_hold, t_max, on_Trackbar_3);
    createTrackbar("rho", windowname, &param1, p1_max, on_Trackbar_4);
    createTrackbar("theta", windowname, &param2, p2_max, on_Trackbar_5);
    createTrackbar("threshold", windowname, &param3, p3_max, on_Trackbar_2);
    createTrackbar("min_d", windowname, &min_d, d_max, on_Trackbar_6);
    createTrackbar("max_d", windowname, &max_d, r_max, on_Trackbar_1);
    
    int capture_width = 1280;
    int capture_height = 720;
    int display_width = 1280;
    int display_height = 720;
    int framerate = 30;
    int flip_method = 0;
    string pipeline = gstreamer_pipeline_1(capture_width,
        capture_height,
        display_width,
        display_height,
        framerate,
        flip_method);
    VideoCapture capture(pipeline, CAP_GSTREAMER);

    Mat image,img;
    int times = 0, Find = 0, l_x = 0, l_y = 0, r_x = 0, r_y = 0;
    Mat BinImg;
    Mat matDst;
    int x1, y1, x2, y2;
    char m[256];
    Uart u;
    std::vector<Vec4i> linesPPHT;


    while (capture.isOpened() && Find < 4)
    {
        times++;  //使摄像头画面稳定

        capture.read(img);
        if (img.empty())break;
        image=img.colRange(280,1000);
        if (waitKey(1) == 27)break;

        if (times >= SLEEP_TIMES)
        {
            if(times==SLEEP_TIMES)printf("START!\n");
            cvtColor(image, matDst, COLOR_BGR2GRAY);
            threshold(matDst, BinImg, t_hold, 255, THRESH_BINARY_INV);

            std::vector<Vec2f> linesSHT;
            //标准霍夫变换检测直线,距离精度为1像素，角度精度为1度，阈值为300
            HoughLines(BinImg, linesSHT, param1, CV_PI / param2, threshold);
            //直线在原图上绘制
            matDst = image.clone();
            printf("%d\n",linesSHT.size())
            for (size_t i = 0; i < linesSHT.size(); i++) {
                //直线的rho和theta值
                float rho = linesSHT[i][0], theta = linesSHT[i][1];
                //pt1和pt2是直线的两个端点，2000是经验值（满足覆盖所有前景点像素）
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a * rho, y0 = b * rho;
                pt1.x = cvRound(x0 + 2000 * (-b)); //把浮点数转化成整数
                pt1.y = cvRound(y0 + 2000 * (a));
                pt2.x = cvRound(x0 - 2000 * (-b));
                pt2.y = cvRound(y0 - 2000 * (a));
                line(matDst, pt1, pt2, Scalar(255), 4);
            }
    }

        }
    }

    capture.release();
    destroyAllWindows();

}

void on_Trackbar_1(int, void*){;}
void on_Trackbar_2(int, void*){;}
void on_Trackbar_3(int, void*){;}
void on_Trackbar_4(int, void*){;}
void on_Trackbar_5(int, void*){;}
void on_Trackbar_6(int, void*){;}