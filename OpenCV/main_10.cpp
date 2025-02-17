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
   return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}


const char* windowname = "win";
int t_hold = 105;
int param1 = 2;   //double类型的rho， 以像素为单位的距离精度。 另一种形容方式是直线搜索时的进步尺寸的单位半径。
int param2 = 90;    //double类型的theta，以弧度为单位的角度精度。另一种形容方式是直线搜索时的进步尺寸的单位角度。
int param3 = 50;   //int类型的threshold，累加平面的阈值参数，即识别某部分为图中的一条直线时它在累加平面中必须达到的值。
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

   //创建显示窗口
   Mat image,img;

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

    int sleep_times = 0, Find = 0, l_x = 0, l_y = 0, r_x = 0, r_y = 0;
    Mat BinImg;
    Mat matDst;
    int x1, y1, x2, y2;
    std::vector<Vec4i> linesPPHT;

    while (capture.isOpened() && Find < 4)
    {
        sleep_times++;  //使摄像头画面稳定

        capture.read(img);
        if (img.empty())break;
       image=img.colRange(280,1000);

        if (waitKey(1) == 27)break;

        if (sleep_times >= 60)
        {
            cvtColor(image, matDst, COLOR_BGR2GRAY);
            threshold(matDst, BinImg, t_hold, 255, THRESH_BINARY_INV);

            //累计概率霍夫变换检测直线，得到的是直线的起止端点
            HoughLinesP(BinImg, linesPPHT, param1, CV_PI / param2, param3, min_d, max_d);

            for (size_t i = 0; i < linesPPHT.size(); i++) {
                //直接绘制直线
                x1 = linesPPHT[i][0], y1 = linesPPHT[i][1], x2 = linesPPHT[i][2], y2 = linesPPHT[i][3];
                line(image, Point(x1, y1), Point(x2, y2), Scalar(0), 1, 8);

                if (x1 < 150 && x2 < 150 && myabs(x2 - x1) < 3 && !l_x)
                {
                    Find++;
                    l_x = (x2 + x1) / 2;
                }
                else if (y1 < 280 && y2 < 280 && myabs(y1 - y2) < 3 && !l_y)
                {
                    Find++;
                    l_y = (y1 + y2) / 2;
                }
                else if (x1 > 500 && x2 > 500 && myabs(x2 - x1) < 3 && !r_x)
                {
                    Find++;
                    r_x = (x2 + x1) / 2;
                }
                else if (y1 > 410 && y2 > 410 && myabs(y1 - y2) < 3 && !r_y)
                {
                    Find++;
                    r_y = (y1 + y2) / 2;
                }
            }
            printf("%d\n", Find);
            imshow("BinImg", BinImg);
            imshow(windowname, image);

        }

    }

    printf("lx:%d lY:%d\nrx:%d rY:%d\n", l_x, l_y, r_x, r_y);
    circle(image, Point((l_x + r_x) / 2, (l_y + r_y) / 2), 5, Scalar(255, 0, 0), 3, LINE_8, 0);
    imwrite("diannsai.png", image);
    system("pause");

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