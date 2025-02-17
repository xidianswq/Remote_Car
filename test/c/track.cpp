#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
using namespace cv;

int main()
{
   VideoCapture cap(0);

   //创建显示窗口
   namedWindow("CSI Camera", WINDOW_AUTOSIZE);
   Mat image;

   //逐帧显示
   while (cap.isOpened())
   {
       cap.read(image);

       if (image.empty())break;
		Mat BinImg;
		threshold(image, BinImg, 150, 255, THRESH_BINARY);
		Mat matCanny;
		Canny(BinImg, matCanny, 100, 300, 3, false);		//canny算子
		Mat matDst = image.clone();

		std::vector<Vec2f> linesSHT;
		//标准霍夫变换检测直线,距离精度为1像素，角度精度为1度，阈值为300
		HoughLines(matCanny, linesSHT, 1, CV_PI / 180, 150);
		//直线在原图上绘制
		matDst = image.clone();
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
			line(matDst, pt1, pt2, Scalar(255), 8);
		}

       imshow("track", matDst);

       if (waitKey(10) == 27)break;
   }

   cap.release();
   destroyAllWindows();
}
