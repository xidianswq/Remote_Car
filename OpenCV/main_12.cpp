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
   return "nvarguscamerasrc exposurecompensation=0 saturation=0 ee-mode=2 ee-strength=1 ! video/x-raw(memory:NVMM), width=(int)" + to_string(capture_width) + ", height=(int)" +
       to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + to_string(framerate) +
       "/1 ! nvvidconv flip-method=" + to_string(flip_method) + " ! video/x-raw, width=(int)" + to_string(display_width) + ", height=(int)" +
       to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

#define SLEEP_TIMES 10
Mat img,image;

const char* windowname = "win";
int t_hold = 100;
int blockSize = 5; //角点检测时的邻域大小
int aperturezie = 6; //用来求取图像梯度的sobel窗口大小
double k = 0.04;  //常数
const int t_max = 255;
const int b_max = 13;
const int a_max = 6;

int Find = 0;
int P[8][2] = { 0 };
int Vertex[4][2] = { 0 };
int sign[8] = { 0 };

int myabs(int a)
{
    return a > 0 ? a : -a;
}

#define MAX_DIS 20
int Is_Exit(int i, int j)
{
    int k = 0;
    for (k = 0; k < Find; k++) 
    {
        if (myabs(P[k][0]-i)+ myabs(P[k][1]-j)<MAX_DIS)return 1;
    }
    return 0;
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
    createTrackbar("blockSize", windowname, &blockSize, b_max, on_Trackbar_4);
    createTrackbar("aperturezie", windowname, &aperturezie, a_max, on_Trackbar_5);

    int times = 0;
    Mat BinImg, GrayImg, CannyImg, CornerImg;
    std::vector<Vec4i> linesPPHT;


   while (capture.isOpened() ) 
    {
        times++;  //使摄像头画面稳定

        capture.read(img);
        image=img.colRange(280,1000);
        
        if (image.empty())break;

        if (times >= SLEEP_TIMES)
        {
            cvtColor(image, GrayImg, COLOR_BGR2GRAY);
            threshold(GrayImg, BinImg, t_hold, 255, THRESH_BINARY_INV|THRESH_OTSU);
            //Canny(BinImg, CannyImg, 100, 300, 3, false);		//canny算子
            //计算图像的Harris角点
            cornerHarris(BinImg, CornerImg, blockSize, 2*aperturezie+1, k);
            //将角点响应进行归一化到0~255范围内
            normalize(CornerImg, CornerImg, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
            //在彩色图像用红色圆圈绘制出响应较大的角点位置
            for (int j = 0; j < CornerImg.rows; j++) {
                for (int i = 0; i < CornerImg.cols; i++) {
                    if (CornerImg.at<float>(j, i) > 150.0f) {
                        circle(image, Point(i, j), 8, Scalar(0, 0, 255), 2, 8, 0);
                        // if (!Is_Exit(i, j))
                        // {                            
                        //     P[Find][0] = i;
                        //     P[Find][1] = j;
                        //     Find++;
                        // }
                    }
                }
            }
            imshow("BinImg", BinImg);
            imshow("CornerImg", CornerImg);
            imshow("windowname", image);

            printf("%d\n", Find);
        }
    }

//     int i,j,k,dis,min = 1000;
//     int temp1, temp2;
//     for (k=0,i = 0; i < 8; i++)
//     {
//         if (sign[i])continue;
//         min = 2000;
//         for (j = 0; j < 8; j++)
//         {
//             if (i == j||sign[j])continue;
//             dis = myabs(P[i][0] - P[j][0]) + myabs(P[i][1] - P[j][1]);
//             if (dis< min)
//             {
//                 min = dis;
//                 temp1 = i;
//                 temp2 = j;
//             }
//         }
//         sign[temp1] = 1;
//         sign[temp2] = 1;
//         Vertex[k][0] = (P[temp1][0] + P[temp2][0])/2;
//         Vertex[k][1] = (P[temp1][1] + P[temp2][1])/2;
//         k++;
//     }
//     for (i = 0; i < 4; i++)
//     {
//         circle(image, Point(Vertex[i][0], Vertex[i][1]), 8, Scalar(0, 0, 255), 2, 8, 0);
//         printf("%d %d\n", Vertex[i][0], Vertex[i][1]);
//     }
//     imwrite("diansai_1.png", image);

//    capture.release();
//    destroyAllWindows();
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
