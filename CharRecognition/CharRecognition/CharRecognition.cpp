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

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	string imgName = "D:/实习/图片/pic/2017.08.25/C1-08251718/C1_288.bmp";
	Mat srcImg = imread(imgName, 0);
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
	vector<RotatedRect> rotatedRects;
	mip->FindTextRegion(morphologyImg, &rotatedRects, 3600, 25000);

	//画出矩形
	mip->DrawRects(&srcImg, rotatedRects, false);

	//计算文本区域旋转角度
	//cout << rects.size() << endl;
	float angle=0.0;
	if (rotatedRects.size() != 1)
	{
		cout << "找到" << rotatedRects.size() << "个文本区域" << endl;
		return 0;
	}
	else
	{
		angle = mip->CalculateAngle(rotatedRects);
	}

	//旋转图像，得到水平文字
	Mat rotatedImg;
	Mat M = getRotationMatrix2D(rotatedRects[0].center, angle, 1);
	warpAffine(srcImg, rotatedImg, M, srcImg.size());

	//阈值分割
	Mat binaryImgRotated;
	threshold(rotatedImg, binaryImgRotated, 180, 255, 1);

	//形态学处理
	Mat morphologyImgRotated;
	Mat ellElement2 = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImgRotated, morphologyImgRotated, ellElement2);

	//寻找字符区域
	vector<RotatedRect> rotatedRectsCharRegion;
	mip->FindTextRegion(morphologyImgRotated, &rotatedRectsCharRegion, 3600, 8000, false, true);

	//画出矩形
	mip->DrawRects(&rotatedImg, rotatedRectsCharRegion, false);

	//缩小字符区域
	int topLeft_x = rotatedRectsCharRegion[0].center.x - rotatedRectsCharRegion[0].size.width / 2;
	int topLeft_y = rotatedRectsCharRegion[0].center.y - rotatedRectsCharRegion[0].size.height / 2;
	Rect CharRegionRect(topLeft_x, topLeft_y, rotatedRectsCharRegion[0].size.width, rotatedRectsCharRegion[0].size.height);
	Mat reducedBinaryImg = binaryImgRotated(CharRegionRect);
	Mat reducedGrayImg = rotatedImg(CharRegionRect);
	//cout << CharRegionRect << endl;

	//分割单个字符
	vector<RotatedRect> rotatedRectsChar;
	mip->FindTextRegion(reducedBinaryImg, &rotatedRectsChar, 30, 300, true, true);

	mip->DrawRects(&reducedBinaryImg, rotatedRectsChar, false, Scalar(127, 127, 127));

	//排序
	Rect sortedRectsChar[15];
	mip->SortMultiRowRects(rotatedRectsChar, sortedRectsChar);

	//单字符识别
	string recoStr = "";
	for (int i = 0; i < rotatedRectsChar.size(); i++)
	{
		Mat singleCharImg = reducedBinaryImg(sortedRectsChar[i]);
		recoStr.push_back(mip->SingleCharReco(singleCharImg, "../../TrainAnn/bpcharModel.xml"));
		cv::namedWindow("result", CV_WINDOW_NORMAL);
		imshow("result", singleCharImg);
		waitKey(500);
		destroyWindow("result");
		/*string name = "6";
		string priorChars = "201708253621719";
		string s(1, priorChars[i]);
		string saveImgName = "../../TrainANN/TrainImages/" + s + "/" + name + to_string(i) + ".png";
		cout << saveImgName << endl;
		imwrite(saveImgName, singleCharImg);*/
	}

	cout << "识别的字符为：" << recoStr << endl;

	//cv::namedWindow("result", CV_WINDOW_NORMAL);
	//imshow("result", reducedBinaryImg);
	//imwrite("D:/实习/图片/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	//waitKey(0);

	return 0;
}

