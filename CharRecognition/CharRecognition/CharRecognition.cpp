// CharRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	Mat srcImg = imread("D:/实习/图片/pic/2017.08.25/C1-08251718/C1_12 (2).bmp", 0);
	if (srcImg.empty())
	{
		std::cout << "打开图片失败,请检查" << std::endl;
		return -1;
	}
	
	MyImgProc *mip = new MyImgProc();

	//预处理：滤波，阈值分割
	Mat emphasizeImg;
	mip->Emphasize(srcImg, &emphasizeImg, 7, 7, 5);
	Mat binaryImg;
	threshold(emphasizeImg, binaryImg, 190, 255, 1);

	//形态学处理
	Mat morphologyImg;
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImg, morphologyImg, ellElement);

	//寻找文字区域
	vector<RotatedRect> rects;
	mip->FindTextRegion(morphologyImg, &rects, 3600, 25000);

	//画出矩形
	mip->drawRects(&srcImg, rects, false);

	//计算文本区域旋转角度
	//cout << rects.size() << endl;
	float angle=0.0;
	if (rects.size() != 1)
	{
		cout << "找到"<<rects.size()<<"个文本区域" << endl;
		return 0;
	}
	else
	{
		angle = mip->calculateAngle(rects);
	}

	//旋转图像，得到水平文字
	Mat rotatedImg;
	Mat M = getRotationMatrix2D(rects[0].center, angle, 1);
	warpAffine(srcImg, rotatedImg, M, srcImg.size());

	//阈值分割
	Mat binaryImgRotated;
	threshold(rotatedImg, binaryImgRotated, 180, 255, 1);

	//形态学处理
	Mat morphologyImgRotated;
	Mat ellElement2 = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImgRotated, morphologyImgRotated, ellElement2);

	//寻找字符区域
	vector<RotatedRect> rectsCharRegion;
	mip->FindTextRegion(morphologyImgRotated, &rectsCharRegion, 3600, 8000, false, true);

	//画出矩形
	mip->drawRects(&rotatedImg, rectsCharRegion, false);

	//缩小字符区域
	int topLeft_x = rectsCharRegion[0].center.x - rectsCharRegion[0].size.width / 2;
	int topLeft_y = rectsCharRegion[0].center.y - rectsCharRegion[0].size.height / 2;
	Rect CharRegionRect(topLeft_x, topLeft_y, rectsCharRegion[0].size.width, rectsCharRegion[0].size.height);
	Mat reducedImg = binaryImgRotated(CharRegionRect);
	//cout << CharRegionRect << endl;

	//分割单个字符
	vector<RotatedRect> charRects;
	mip->FindTextRegion(reducedImg, &charRects, 30, 300, true, true);

	mip->drawRects(&reducedImg, charRects, true, Scalar(127,127,127));

	cv::namedWindow("result", CV_WINDOW_NORMAL);
	imshow("result", reducedImg);
	//imwrite("D:/实习/图片/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	waitKey(0);

	return 0;
}

