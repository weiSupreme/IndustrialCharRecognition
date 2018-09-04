// TrainSVM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <io.h>
#include <string>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace ml;
using namespace std;

//参考自博文：https://blog.csdn.net/chaipp0607/article/details/68067098/ 
int _tmain(int argc, _TCHAR* argv[])
{
	////==========================读取图片创建训练数据==============================////
	const int imageRows = 32;
	const int imageCols = 16;
	const int classSum = 10;

	Mat classes;
	Mat trainingData;
	Mat trainingImages;
	vector<int> trainingLabels;

	Mat sampleFeatureMat;
	//检测窗口(64,128),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9
	HOGDescriptor hog(Size(16, 32), Size(8, 8), Size(4, 4), Size(4, 4), 5);
	sampleFeatureMat = Mat::zeros(1, 420, CV_32FC1);

	Mat src, resizeImg, trainImg;
	std::cout << "准备数据..." << std::endl;
	for (int i = 0; i < classSum; i++)
	{
		//目标文件夹路径
		//std::string inPath = "D:/实习/seg/datasets/cvSamplesDigits/";
		std::string inPath = "D:/实习/seg/TrainImagesGray/";
		char temp[256];
		int k = 0;
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
			//std::string imgname = "D:/实习/seg/datasets/cvSamplesDigits/";
			std::string imgname = "D:/实习/seg/TrainImagesGray/";
			imgname = imgname + temp + "/" + fileinfo.name;
			src = imread(imgname, 0);
			if (src.empty())
			{
				std::cout << "can not load image \n" << std::endl;
				return -1;
			}
			resize(src, resizeImg, Size(imageCols, imageRows), (0, 0), (0, 0), INTER_AREA);
			//threshold(resizeImg, resizeImg, 0, 255, THRESH_BINARY | THRESH_OTSU);
			//resizeImg = resizeImg.reshape(1, 1);
			//trainingImages.push_back(resizeImg);

			vector<float> descriptors;//HOG描述子向量
			hog.compute(resizeImg, descriptors, Size(4, 4));//计算HOG描述子，检测窗口移动步长(8,8)
			for (int i = 0; i<420; i++)
				sampleFeatureMat.at<float>(0, i) = descriptors[i];
			trainingImages.push_back(sampleFeatureMat);

			// 设置标签数据
			trainingLabels.push_back(i);
			k++;

		} while (!_findnext(handle, &fileinfo));

		_findclose(handle);
	}

	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);
	//训练数据及标签
	
	////==========================训练部分==============================////
	std::cout << "创建模型" << std::endl;
	// Set up SVM's parameters
	Ptr<SVM> svm = SVM::create();
	//Ptr<SVM> svm = StatModel::load<SVM>("../svm.xml");
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR); //LINEAR POLY
	//svm->setKernel(SVM::POLY);
	//svm->setDegree(1.0);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 3000, 1e-6));

	std::cout << "开始训练" << std::endl;
	// Train the SVM
	svm->train(trainingData, cv::ml::ROW_SAMPLE, classes);
	//svm->trainAuto(trainDataSet);
	std::cout << "训练完成" << std::endl;
	svm->save("../svm.xml");
	return 0;
}

