#if !defined MYIMGPROC_H
#define MYIMGPROC_H

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2\ml.hpp>


class MyImgProc
{

public:
	MyImgProc();
	~MyImgProc();

	void Emphasize(cv::Mat src, cv::Mat* dst, int maskwidth=5, int maskheight=5, float factor=1.0);
	void Morphology(cv::Mat src, cv::Mat* dst);
	
	void FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rRects, int areaMin, int areaMax, bool externalFlag=false, bool horizontalRectFlag=false);
	void DrawRects(cv::Mat* src, std::vector<cv::RotatedRect> rRects, bool showFlag=false, cv::Scalar color = cv::Scalar(0, 255, 0));
	void SortMultiRowRects(std::vector<cv::RotatedRect> rRects, cv::Rect* rects, int row=2);
	void SortSingleRowRects(std::vector<cv::RotatedRect> rRects, cv::Rect* rects, int num);
	
	void RotateImage(cv::Mat src, cv::Mat* dst, std::vector<cv::RotatedRect> rRects, float angle=0.0);
	float CalculateAngle(std::vector<cv::RotatedRect> rRects);
	
	void SingleCharRecoANN(cv::Mat src, int* charIndex, float* confidence, cv::Ptr<cv::ml::ANN_MLP> model, int resizeWidth = 8, int resizeHeight = 16);
	void MultiCharRecoANN(cv::Mat src, int* charIndexs, float* confidence, cv::Rect* rects, int charNum, std::string model, bool showSingleCharflag = false, int waitTime = 800);

	void SingleCharRecoSVM(cv::Mat src, int* prediction, cv::Ptr<cv::ml::SVM> model, int resizeWidth = 8, int resizeHeight = 16);
	void MultiCharRecoSVM(cv::Mat src, int* predictions, cv::Rect* rects, int charNum, std::string model, bool showSingleCharflag = false, int waitTime = 800);
};

#endif