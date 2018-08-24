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
	mip->Morphology(binaryImg, &morphologyImg);

	//寻找文字区域
	vector<RotatedRect> rects;
	mip->FindTextRegion(morphologyImg, &rects);

	//画出轮廓
	if (0)
	{
		for each (RotatedRect rect in rects)
		{
			Point2f P[4];
			rect.points(P);
			for (int j = 0; j <= 3; j++)
			{
				line(srcImg, P[j], P[(j + 1) % 4], Scalar(0, 255, 0), 2);
			}
		}
	}

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
		angle = rects[0].angle;
		if (0 < abs(angle) && abs(angle) <= 45)  //逆时针
			angle = angle;
		else if (45 < abs(angle) && abs(angle) < 90) //顺时针
			angle = 90 - abs(angle);
	}

	//旋转图像，得到水平文字
	Mat rotatedImg;
	Mat M = getRotationMatrix2D(rects[0].center, angle, 1);
	warpAffine(srcImg, rotatedImg, M, srcImg.size());

	imshow("img", rotatedImg);
	//imwrite("D:/实习/图片/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	waitKey(0);

	return 0;
}

