//-----------------------------------【程序说明】----------------------------------------------
//		程序名称:：《基于opencv和C++的工业字符检测》 博文配套源码 
//		开发所用IDE版本：Visual Studio 2013
//		开发所用OpenCV版本：	3.4
//		2018年8月18日 Created by zhuwei
//		github地址：https://github.com/weiSupreme/IndustrialCharRecognition.git  
//		博客：https://weiSupreme.github.io 
//----------------------------------------------------------------------------------------------
 
// CharRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"
#include<string>
#include<windows.h>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	//string imgName = "images/C1_12 (2).bmp";
	string imgName = "D:/实习/图片/pic/2017.08.25/C1-08251718/C1_128 (2).bmp";
	Mat srcImg = imread(imgName, 0);
	if (srcImg.empty())
	{
		std::cout << "打开图片失败,请检查。图片存放在 ‘IndustrialCharRecognition\CharRecognition\CharRecognition\images’ " << std::endl;
		return -1;
	}

	long t1 = GetTickCount();

	MyImgProc *mip = new MyImgProc();

	//预处理：滤波，阈值分割
	Mat emphasizeImg;
	//mip->Emphasize(srcImg, &emphasizeImg, 7, 7, 5);
	medianBlur(srcImg, emphasizeImg, 3);
	Mat binaryImg;
	threshold(emphasizeImg, binaryImg, 200, 255, 1);
	
	//形态学处理
	Mat morphologyImg;
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImg, morphologyImg, ellElement);

	//寻找文字区域
	vector<RotatedRect> rotatedRects;
	mip->FindTextRegion(morphologyImg, &rotatedRects, 3600, 25000);
	if (rotatedRects.size() != 1)
	{
		cout << "找到" << rotatedRects.size() << "个文本区域" << endl;
		return 0;
	}

	//画出矩形
	mip->DrawRects(&srcImg, rotatedRects, false);

	//计算文本区域旋转角度
	float angle=0.0;
	angle = mip->CalculateAngle(rotatedRects);

	//获取水平矩形
	Rect horizontalRect = rotatedRects[0].boundingRect();
	Mat reducedGrayImg = srcImg(horizontalRect);

	//旋转图像，得到水平文字
	Mat rotatedGrayImg;
	mip->RotateImage(reducedGrayImg, &rotatedGrayImg, rotatedRects, angle);

	//阈值处理
	Mat rotatedBinaryImg;
	threshold(rotatedGrayImg, rotatedBinaryImg, 190, 255, 1);
	
	//形态学处理
	Mat morphologyImgRotated;
	Mat ellElement2 = getStructuringElement(MORPH_RECT, Size(1, 2));
	dilate(rotatedBinaryImg, morphologyImgRotated, ellElement2);

	//分割单个字符
	vector<RotatedRect> rotatedRectsChar;
	mip->FindTextRegion(morphologyImgRotated, &rotatedRectsChar, 10, 600, true, true);

	mip->DrawRects(&rotatedGrayImg, rotatedRectsChar, true, Scalar(127, 127, 127));

	//排序和识别
	const int charNum = 15;
	int index[charNum];
	float conf[charNum];
	Rect sortedRectsChar[charNum];
	mip->SortMultiRowRects(rotatedRectsChar, sortedRectsChar);
	mip->MultiCharReco(rotatedGrayImg, index, conf, sortedRectsChar, rotatedRectsChar.size(), "../../TrainAnn/bpcharModel.xml");
	
	long t2 = GetTickCount();
	cout << "处理时间：" << (t2 - t1 - 78) << "ms" << endl;
	string totalChar = "0123456789";
	for (byte i = 0; i < charNum; i++)
	{
		cout << "识别的字符为：" << totalChar[index[i]]<<"   ";
		cout << "置信度为：" << conf[i] << endl;
	}

	return 0;
}

