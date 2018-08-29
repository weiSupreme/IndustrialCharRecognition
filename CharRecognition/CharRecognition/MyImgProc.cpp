#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"
#include <opencv2\ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

MyImgProc::MyImgProc(){}
MyImgProc::~MyImgProc(){}

//增强图像中的高频区域（边缘和角点），移植自halcon emphasize函数。实现效果与halcon的区别较大
void MyImgProc::Emphasize(cv::Mat src, cv::Mat* dst, int maskwidth, int maskheight, float factor)
{
	blur(src, *dst, Size(maskwidth, maskheight));
	subtract(src, *dst, *dst);
	scaleAdd(*dst, factor, src, *dst);
}

//形态学操作
void MyImgProc::Morphology(cv::Mat src, cv::Mat* dst)
{
	Mat recElement = getStructuringElement(MORPH_RECT, Size(30, 9));
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(src, *dst, ellElement);
	//erode(*src, *dst, DilationElement);
	//dilate(*src, *dst, DilationElement);
}

//获取文字区域。参考自：https://blog.csdn.net/lgh0824/article/details/76100599
void MyImgProc::FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rects, int areaMin, int areaMax, bool externalFlag, bool horizontalRectFlag)
{
	//1.查找轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	if (externalFlag)
	{
		findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	}
	else
	{
		findContours(src, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
	}

	//2.筛选那些面积小的
	for (int i = 0; i < contours.size(); i++)
	{
		//计算当前轮廓的面积
		double area = contourArea(contours[i]);

		//面积小于1000的全部筛选掉
		if (area < areaMin || area > areaMax)
			continue;

		//轮廓近似，作用较小，approxPolyDP函数有待研究
		double epsilon = 0.001*arcLength(contours[i], true);
		Mat approx;
		approxPolyDP(contours[i], approx, epsilon, true);

		if (horizontalRectFlag)
		{
			Rect rectHoriz = boundingRect(contours[i]);
			float center_x = rectHoriz.x + rectHoriz.width / 2;
			float center_y = rectHoriz.y + rectHoriz.height / 2;
			RotatedRect rect(Point2f(center_x, center_y), Size2f(rectHoriz.width, rectHoriz.height), 0);
			rects->push_back(rect);
		}
		else
		{
			//找到最小矩形，该矩形可能有方向
			RotatedRect rect = minAreaRect(contours[i]);

			//计算高和宽
			//int m_width = rect.boundingRect().width;
			//int m_height = rect.boundingRect().height;

			//符合条件的rect添加到rects集合中
			rects->push_back(rect);
		}

	}
	//return rects;
}

void MyImgProc::DrawRects(cv::Mat* src, std::vector<cv::RotatedRect> rects, bool showFlag, cv::Scalar color)
{
	if (showFlag)
	{
		for each (RotatedRect rect in rects)
		{
			Point2f P[4];
			rect.points(P);
			for (int j = 0; j <= 3; j++)
			{
				line(*src, P[j], P[(j + 1) % 4], color, 1);
			}
		}
	}
}

void MyImgProc::SortMultiRowRects(std::vector<cv::RotatedRect> rotatedRects, cv::Rect* rect, int row)
{
	int center_y = 0;
	for (int i = 0; i < rotatedRects.size(); i++)
	{
		center_y += rotatedRects[i].center.y;
	}
	center_y /= rotatedRects.size();
	int *indexRow[2];
	int row1Cnt = 0, row2Cnt = 0;
	for (int i = 0; i < rotatedRects.size(); i++)
	{
		if (rotatedRects[i].center.y < center_y)
		{
			*(indexRow[0]++) = i;
			row1Cnt++;
		}
		else
		{
			*(indexRow[1]++) = i;
			row2Cnt++;
		}
	}
}

void MyImgProc::SortSingleRowRects(std::vector<cv::RotatedRect> rotatedRects, cv::Rect* rect)
{

}

float MyImgProc::CalculateAngle(std::vector<cv::RotatedRect> rect)
{
	float angle = rect[0].angle;
	if (0 < abs(angle) && abs(angle) <= 45)  //逆时针
		angle = angle;
	else if (45 < abs(angle) && abs(angle) < 90) //顺时针
		angle = 90 - abs(angle);
	return angle;
}

char MyImgProc::SingleCharReco(cv::Mat src, std::string filePath, int resizeWidth, int resizeHeight)
{
	//读取模型
	Ptr<ANN_MLP> model = StatModel::load<ANN_MLP>(filePath);

	Mat dst;
	//将测试图像转化为1*128的向量
	resize(src, dst, Size(resizeWidth, resizeHeight), (0, 0), (0, 0), INTER_AREA);
	Mat_<float> srcMat(1, resizeWidth*resizeHeight);
	for (int i = 0; i < resizeWidth*resizeHeight; i++)
	{
		srcMat.at<float>(0, i) = (float)dst.at<uchar>(i / 8, i % 8);
	}
	
	//使用训练好的MLP model预测测试图像
	model->predict(srcMat, dst);

	//选出
	double maxVal = 0;
	Point maxLoc;
	minMaxLoc(dst, NULL, &maxVal, NULL, &maxLoc);
	std::cout << "预测结果：" << maxLoc.x << " 置信度:" << maxVal * 100 << "%" << std::endl;

	//imshow("char image", src);
	//waitKey(1000);
	char tmp='o';
	return tmp;
}