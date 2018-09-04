#if !defined MYIMGPROC_H
#define MYIMGPROC_H

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2\ml.hpp>
#include <opencv2\opencv.hpp>


class MyImgProc
{

public:
	MyImgProc();
	~MyImgProc();

	void Emphasize(const cv::Mat &src, cv::Mat &dst, int maskwidth=5, int maskheight=5, float factor=1.0);
	void Morphology(const cv::Mat &src, cv::Mat &dst);
	
	void FindTextRegion(const cv::Mat &src, std::vector<cv::RotatedRect> &rRects, int areaMin, int areaMax, bool externalFlag=false, bool horizontalRectFlag=false);
	void DrawRects(const cv::Mat &src, std::vector<cv::RotatedRect> &rRects, bool showFlag=false, cv::Scalar color = cv::Scalar(0, 255, 0));
	void SortMultiRowRects(std::vector<cv::RotatedRect> &rRects, cv::Rect* rects, int row=2);
	void SortSingleRowRects(std::vector<cv::RotatedRect> &rRects, cv::Rect* rects, int num);
	
	void RotateImage(const cv::Mat &src, cv::Mat &dst, float angle=0.0);
	float CalculateAngle(const std::vector<cv::RotatedRect> &rRects);
	
	void SingleCharRecoANN(const cv::Mat &src, int &charIndex, float &confidence, cv::Ptr<cv::ml::ANN_MLP> &model, int resizeWidth = 8, int resizeHeight = 16);
	void MultiCharRecoANN(const cv::Mat &src, int* charIndexs, float* confidence, cv::Rect* rects, int charNum, std::string model, bool showSingleCharflag = false, int waitTime = 800);

	void SingleCharRecoSVM(const cv::Mat &src, cv::Mat &sampleFeatureMat, cv::HOGDescriptor &hog, int &prediction, cv::Ptr<cv::ml::SVM> &model, int resizeWidth = 16, int resizeHeight = 32);
	void MultiCharRecoSVM(const cv::Mat &src, int* predictions, cv::Rect* rects, int charNum, std::string model, bool showSingleCharflag = false, int waitTime = 800);
};

#endif