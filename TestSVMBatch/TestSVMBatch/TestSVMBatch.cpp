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

void SingleCharRecoSVM(const Mat &src, Mat &sampleFeatureMat, HOGDescriptor &hog, int &prediction, Ptr<SVM> &model, int resizeWidth, int resizeHeight);

#define HogFeatureDim 60
int _tmain(int argc, _TCHAR* argv[])
{
	Mat src;
	int total = 0, right = 0, timeall = 0;
	Mat sampleFeatureMat;
	//检测窗口(64,128),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9
	HOGDescriptor hog(Size(8, 16), Size(8, 8), Size(4, 4), Size(4, 4), 5);
	sampleFeatureMat = Mat::zeros(1, HogFeatureDim, CV_32FC1);

	Ptr<SVM> svmModel = StatModel::load<SVM>("../../TrainSVM/svm.xml");

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
			SingleCharRecoSVM(src, sampleFeatureMat, hog, prediction, svmModel, 8, 16);
			long t2 = GetTickCount();
			timeall += (t2 - t1);
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
	cout << "平均用时为： " << timeall / total << " ms" << endl;
	return 0;
}

/* 识别单个字符， SVM
* @param src 含有字符的图像
* @param sampleFeatureMat hog特征矩阵
* @param hog hog描述子
* @param prediction 预测字符类别
* @param model SVM模型
* @param resizeWidth 图像放缩尺寸
* @param resizeHeight 图像放缩尺寸
*/
void SingleCharRecoSVM(const Mat &src, Mat &sampleFeatureMat, HOGDescriptor &hog, int &prediction, Ptr<SVM> &model, int resizeWidth, int resizeHeight)
{
	Mat dst;
	//将测试图像转化为1*128的向量
	resize(src, dst, Size(resizeWidth, resizeHeight), (0, 0), (0, 0), INTER_AREA);
	vector<float> descriptors;//HOG描述子向量
	hog.compute(dst, descriptors, Size(4, 4));//计算HOG描述子，检测窗口移动步长(8,8)
	for (int i = 0; i < HogFeatureDim; i++)
		sampleFeatureMat.at<float>(0, i) = descriptors[i];

	//Mat p = dst.reshape(1, 1);
	//p.convertTo(p, CV_32FC1);

	float response = model->predict(sampleFeatureMat);

	//cout << "预测结果：" << response << endl;
	prediction = response;
}

