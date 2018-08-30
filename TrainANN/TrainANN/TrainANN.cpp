// TrainANN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <io.h>
#include <string>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace ml;

//参考自博文：https://blog.csdn.net/akadiao/article/details/79236458 
int _tmain(int argc, _TCHAR* argv[])
{
	////==========================读取图片创建训练数据==============================////
	const int imageRows = 16;
	const int imageCols = 8;
	const int classSum = 10;
	const int imagesSum = 116;
	float trainingData[classSum*imagesSum][imageRows*imageCols] = { { 0 } };//每一行一个训练样本
	float labels[classSum*imagesSum][classSum] = { { 0 } };//训练样本标签
	Mat src, resizeImg, trainImg;
	std::cout << "准备数据..." << std::endl;
	for (int i = 0; i < classSum; i++)
	{
		//目标文件夹路径
		std::string inPath = "D:/实习/seg/TrainImages/";
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
			std::string imgname = "D:/实习/seg/TrainImages/";
			imgname = imgname + temp + "/" + fileinfo.name;
			src = imread(imgname, 0);
			if (src.empty())
			{
				std::cout << "can not load image \n" << std::endl;
				return -1;
			}
			resize(src, resizeImg, Size(imageCols, imageRows), (0, 0), (0, 0), INTER_AREA);
			//二值化
			//threshold(resizeImg, trainImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			//imshow("image", trainImg);
			//waitKey(5);
			for (int j = 0; j<imageRows*imageCols; j++)
			{
				trainingData[i*imagesSum + k][j] = (float)resizeImg.data[j];
			}
			// 设置标签数据
			for (int j = 0; j < classSum; j++)
			{
				if (j == i)
					labels[i*imagesSum + k][j] = 1;
				else
					labels[i*imagesSum + k][j] = 0;
			}
			k++;

		} while (!_findnext(handle, &fileinfo));
		Mat labelsMat(classSum*imagesSum, classSum, CV_32FC1, labels);

		_findclose(handle);
	}
	//训练数据及标签
	Mat trainingDataMat(classSum*imagesSum, imageRows*imageCols, CV_32FC1, trainingData);
	Mat labelsMat(classSum*imagesSum, classSum, CV_32FC1, labels);
	//std::cout<<"trainingDataMat: \n"<<trainingDataMat<<"\n"<<std::endl;
	//std::cout<<"labelsMat: \n"<<labelsMat<<"\n"<<std::endl;
	////==========================训练部分==============================////
	std::cout << "创建模型" << std::endl;
	Ptr<ANN_MLP>model = ANN_MLP::create();
	//Ptr<ANN_MLP> model = StatModel::load<ANN_MLP>("../bpcharModel.xml");
	Mat layerSizes = (Mat_<int>(1, 3) << imageRows*imageCols, imageRows*imageCols*2, classSum);
	model->setLayerSizes(layerSizes);
	model->setTrainMethod(ANN_MLP::BACKPROP, 0.001, 0.1);
	model->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1.0, 1.0);
	model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 10000, 0.0001));

	Ptr<TrainData> trainData = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
	std::cout << "开始训练" << std::endl;
	model->train(trainData);
	model->save("../bpcharModel.xml"); //save classifier
	std::cout << "训练完成" << std::endl;
	return 0;
}

