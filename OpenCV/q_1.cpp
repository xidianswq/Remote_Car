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
int param1 = 2;   //double类型的rho， 以像素为单位的距离精度。 另一种形容方式是直线搜索时的进步尺寸的单位半径。
int param2 = 90;    //double类型的theta，以弧度为单位的角度精度。另一种形容方式是直线搜索时的进步尺寸的单位角度。
int param3 = 50;   //int类型的threshold，累加平面的阈值参数，即识别某部分为图中的一条直线时它在累加平面中必须达到的值。
int min_d = 50;     //double类型的minLineLength，有默认值0，表示最低线段的长度，比这个设定参数短的线段就不能被显现出来。
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

void crossline(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,int cross[2])
{
    cross[0]=(y3*x4*x2-y4*x3*x2-y3*x4*x1+y4*x3*x1-y1*x2*x4+y2*x1*x4+y1*x2*x3-y2*x1*x3)/(x4*y2-x4*y1-x3*y2+x3*y1-x2*y4+x2*y3+x1*y4-x1*y3);
    cross[1]=(-y3*x4*y2+y4*x3*y2+y3*x4*y1-y4*x3*y1+y1*x2*y4-y1*x2*y3-y2*x1*y4+y2*x1*y3)/(y4*x2-y4*x1-y3*x2+x1*y3-y2*x4+y2*x3+y1*x4-y1*x3);
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
    int l[2][2],r[2][2],u[2][2],d[2][2];
    Mat BinImg;
    Mat matDst;
    int x1, y1, x2, y2;
    char m[256];
    Uart uart;
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

            HoughLinesP(BinImg, linesPPHT, param1, CV_PI / param2, param3, min_d, max_d);
            for (size_t i = 0; i < linesPPHT.size(); i++) {
                x1 = linesPPHT[i][0], y1 = linesPPHT[i][1], x2 = linesPPHT[i][2], y2 = linesPPHT[i][3];
                line(image, Point(x1, y1), Point(x2, y2), Scalar(0), 1, 8);

                if (x1 < 150 && x2 < 150 && myabs(x2 - x1) < 3 && !l_x){Find++;l_x = (x2 + x1) / 2;l[0][0]=x1;l[0][1]=y1;l[1][0]=x2;l[1][1]=y2;}
                else if (y1 < 150 && y2 < 150 && myabs(y1 - y2) < 3 && !l_y){Find++;l_y = (y1 + y2) / 2;u[0][0]=x1;u[0][1]=y1;u[1][0]=x2;u[1][1]=y2;}
                else if (x1 > 570 && x2 > 570 && myabs(x2 - x1) < 3 && !r_x){Find++;r_x = (x2 + x1) / 2;r[0][0]=x1;r[0][1]=y1;r[1][0]=x2;r[1][1]=y2;}
                else if (y1 > 570 && y2 > 570 && myabs(y1 - y2) < 3 && !r_y){Find++;r_y = (y1 + y2) / 2;d[0][0]=x1;d[0][1]=y1;d[1][0]=x2;d[1][1]=y2;}
            }
            printf("%d\n", Find);
            imshow("BinImg", BinImg);
            imshow(windowname, image);
        }
    }
    int ul[2],ur[2],dl[2],dr[2],ce[2];

    crossline(l[0][0],l[0][1],l[1][0],l[1][1],u[0][0],u[0][1],u[1][0],u[1][1],ul);
    crossline(r[0][0],r[0][1],r[1][0],r[1][1],u[0][0],u[0][1],u[1][0],u[1][1],ur);
    crossline(l[0][0],l[0][1],l[1][0],l[1][1],d[0][0],d[0][1],d[1][0],d[1][1],dl);
    crossline(r[0][0],r[0][1],r[1][0],r[1][1],d[0][0],d[0][1],d[1][0],d[1][1],dr);

    crossline(ul[0],ul[1],dr[0],dr[1],ur[0],ur[1],dl[0],dl[1],ce);

    circle(image, Point(ce[0], ce[1]), 5, Scalar(255, 0, 0), 3, LINE_8, 0);

    sprintf(m,"#a%03db%03dc%03dd%03de%03df%03dg%03dh%03d$\n",ul[0],ul[1],ur[0],ur[1],dr[0],dr[1],dl[0],dl[1]);
    uart.sendUart(m);
    usleep(50000);
    sprintf(m,"#i%03dj%03d$\n",ce[0],ce[1]);
    uart.sendUart(m);

    // sprintf(m,"#a%03db%03dc%03dd%03de%03df%03dg%03dh%03di%03dj%03d$\n",ul[0],ul[1],ur[0],ur[1],dr[0],dr[1],dl[0],dl[1],ce[0],ce[1]);
    // uart.sendUart(m);

    imwrite("铅笔线识别.png", image);
    sleep(2);
    // printf("ENTER ANY KEYS TO CONTINUE\n");
    // waitKey(0);
    // printf("MESSAGE SEND ALREADY\n");
    // printf("ENTER ANY KEYS TO CONTINUE\n");
    // waitKey(0);
    
    capture.release();
    destroyAllWindows();



    // printf("READY TO SEND POINT POSITION! ENTER ESC TO EXIT\n");
    // printf("ENTER ANY KEYS TO CONTINUE\n");
    // waitKey(0);
    usleep(5000);

    pipeline = gstreamer_pipeline_2(capture_width,
        capture_height,
        display_width,
        display_height,
        framerate,
        flip_method);
    capture.open(pipeline, CAP_GSTREAMER);
    times = 0;

    int i=0;
    while (capture.isOpened())
    {
        times++;  //使摄像头画面稳定

        capture.read(img);
        if (img.empty())break;
        image=img.colRange(280,1000);
        if (waitKey(1) == 27)break;

        if (times >=SLEEP_TIMES )
        {
            i++;
            if(times==SLEEP_TIMES)printf("START!\n");
            Mat HsvImg;
            cvtColor(image, HsvImg, COLOR_BGR2HSV);
            Point centre_r = color_recognite(image, Scalar(170, 66, 66), Scalar(180, 255, 255));
            //Point centre_r = color_recognite(image, Scalar(30, 83, 24), Scalar(75, 255, 255));
            if(centre_r.x&&centre_r.y)sprintf(m,"#x%03dy%03d$\n",centre_r.x,centre_r.y);
            uart.sendUart(m);
            printf("%d",i);
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