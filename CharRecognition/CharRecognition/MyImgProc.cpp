#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"

using namespace cv;
using namespace std;

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
	Mat ellationElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(src, *dst, ellationElement);
	//erode(*src, *dst, DilationElement);
	//dilate(*src, *dst, DilationElement);
}

//获取文字区域。参考自：https://blog.csdn.net/lgh0824/article/details/76100599
void MyImgProc::FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rects)
{
	//vector<RotatedRect> rects;
	//1.查找轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//2.筛选那些面积小的
	for (int i = 0; i < contours.size(); i++)
	{
		//计算当前轮廓的面积
		double area = contourArea(contours[i]);

		//面积小于1000的全部筛选掉
		if (area < 3600 || area > 25000)
			continue;

		//轮廓近似，作用较小，approxPolyDP函数有待研究
		double epsilon = 0.001*arcLength(contours[i], true);
		Mat approx;
		approxPolyDP(contours[i], approx, epsilon, true);

		//找到最小矩形，该矩形可能有方向
		RotatedRect rect = minAreaRect(contours[i]);

		//计算高和宽
		int m_width = rect.boundingRect().width;
		int m_height = rect.boundingRect().height;

		//筛选那些太细的矩形，留下扁的
		if (m_height > m_width * 1.2)
			continue;

		//符合条件的rect添加到rects集合中
		rects->push_back(rect);

	}
	//return rects;
}