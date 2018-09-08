// TestSVMBatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include <io.h>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<string>
#include<windows.h>
#include <opencv2\opencv.hpp>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

void SingleCharRecoANN(const Mat &src, int &charIndex, float &confidence, Ptr<ANN_MLP> &model, int resizeWidth, int resizeHeight);

#define HogFeatureDim 60
int _tmain(int argc, _TCHAR* argv[])
{
	Mat src;
	int total = 0, right = 0, timeall = 0;

	Ptr<ANN_MLP> annModel = StatModel::load<ANN_MLP>("../../TrainANN/ann.xml");

	std::cout << "准备测试..." << std::endl;
	for (int i = 0; i < 10; i++)
	{
		//目标文件夹路径
		std::string inPath = "D:/实习/seg/test/dataset/gray/";
		char temp[256];
		sprintf_s(temp, "%d", i);
		inPath = inPath + temp + "/*.png";
		//用于查找的句柄
		long long handle;
		struct _finddata_t fileinfo;
		//第一次查找
		handle = _findfirst(inPath.c_str(), &fileinfo);
		if (handle == -1)
			return -1;
		do
		{
			//找到的文件的文件名
			std::string imgname = "D:/实习/seg/test/dataset/gray/";
			imgname = imgname + temp + "/" + fileinfo.name;
			long t1 = GetTickCount();
			src = imread(imgname, 0);
			if (src.empty())
			{
				std::cout << "can not load image \n" << std::endl;
				return -1;
			}
			int prediction; 
			float conf;
			SingleCharRecoANN(src, prediction, conf, annModel, 8, 16);
			long t2 = GetTickCount();
			timeall += (t2 - t1) * 10;
			if (i == prediction)
				++right;
			++total;
			cout << i << "    :" << prediction << endl;
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
	}
	cout << "总共测试字符数： " << total << endl;
	cout << "正确字符数： " << right << endl;
	cout << "准确率为： " << right * 100 / total << endl;
	cout << "平均用时为： " << timeall / total << " us*100" << endl;
	return 0;
}

/* 识别单个字符， ANN
* @param src 含有字符的图像
* @param charIndex 输出字符所在类别的索引
* @param confidence 输出预测的置信度
* @param model ANN模型
* @param resizeWidth 图像放缩尺寸
* @param resizeHeight 图像放缩尺寸
*/
void SingleCharRecoANN(const Mat &src, int &charIndex, float &confidence, Ptr<ANN_MLP> &model, int resizeWidth, int resizeHeight)
{
	Mat dst;
	//将测试图像转化为1*128的向量
	resize(src, dst, Size(resizeWidth, resizeHeight), (0, 0), (0, 0), INTER_AREA);
	Mat_<float> srcMat(1, resizeWidth*resizeHeight);
	for (int i = 0; i < resizeWidth*resizeHeight; i++)
	{
		srcMat.at<float>(0, i) = (float)dst.at<uchar>(i / resizeWidth, i % resizeWidth);
	}

	//使用训练好的ANN预测图像
	model->predict(srcMat, dst);

	//选出
	double maxVal = 0;
	Point maxLoc;
	minMaxLoc(dst, NULL, &maxVal, NULL, &maxLoc);
	//cout << dst << endl;
	//std::cout << "预测结果：" << maxLoc.x << " 置信度:" << maxVal * 100 << "%" << std::endl;
	confidence = maxVal;
	charIndex = maxLoc.x;
}

