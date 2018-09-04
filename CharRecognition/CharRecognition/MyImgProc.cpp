#include "stdafx.h"
#include "MyImgProc.h"
#include<windows.h>

using namespace cv;
using namespace std;
using namespace ml;

MyImgProc::MyImgProc(){}
MyImgProc::~MyImgProc(){}

//增强图像中的高频区域（边缘和角点），移植自halcon emphasize函数。实现效果与halcon的区别较大
void MyImgProc::Emphasize(const Mat &src, Mat &dst, int maskwidth, int maskheight, float factor)
{
	blur(src, dst, Size(maskwidth, maskheight));
	subtract(src, dst, dst);
	scaleAdd(dst, factor, src, dst);
}

//形态学操作
void MyImgProc::Morphology(const Mat &src, Mat &dst)
{
	Mat recElement = getStructuringElement(MORPH_RECT, Size(30, 9));
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(src, dst, ellElement);
	//erode(src, dst, DilationElement);
	//dilate(src, dst, DilationElement);
}

//获取文字区域。参考自：https://blog.csdn.net/lgh0824/article/details/76100599
/*   搜索文字所在区域
* @param src 输入图像
* @param rRects 存储包围轮廓的带旋转矩形的向量
* @param areaMin 轮廓面积最小值
* @param areaMax 轮廓面积最大值
* @param externalFlag 只寻找最外层轮廓标志位
* @param horizontalRectFlag 只寻找水平矩形标志位
*/
void MyImgProc::FindTextRegion(const Mat &src, vector<RotatedRect> &rRects, int areaMin, int areaMax, bool externalFlag, bool horizontalRectFlag)
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
			rRects.push_back(rect);
		}
		else
		{
			//找到最小矩形，该矩形可能有方向
			RotatedRect rect = minAreaRect(contours[i]);

			//计算高和宽
			//int m_width = rect.boundingRect().width;
			//int m_height = rect.boundingRect().height;

			//符合条件的rect添加到rects集合中
			rRects.push_back(rect);
		}

	}
	//return rects;
}

/*  画出包围轮廓的矩形
* @param src 输入图像
* @param rRects 存储了矩形的向量
* @param showFlag 显示图像标志位
* @param color 指示矩形颜色。例：(0,0,0)
*/
void MyImgProc::DrawRects(const Mat &src, vector<RotatedRect> &rRects, bool showFlag, cv::Scalar color)
{
	if (showFlag)
	{
		Mat image;
		src.copyTo(image);
		for each (RotatedRect rect in rRects)
		{
			Point2f P[4];
			rect.points(P);
			for (int j = 0; j <= 3; j++)
			{
				line(image, P[j], P[(j + 1) % 4], color, 1);
			}
		}
		cv::namedWindow("drawPicture", CV_WINDOW_NORMAL);
		imshow("drawPicture", image);
		waitKey(0);
	}
}

/* 对多行字符的包围矩形进行排序，从上至下，从左至右
* @param rRects 输入的矩形向量
* @param rects 输出的水平矩形向量
* @param row 字符的行数。目前最大支持2行
*/
void MyImgProc::SortMultiRowRects(vector<RotatedRect> &rRects, Rect* rects, int row)
{
	int center_y = 0;
	
	for (int i = 0; i < (rRects).size(); i++)
	{
		center_y += (rRects)[i].center.y;
	}
	center_y /= (rRects).size();
	int row1Index[CharNumMax], row2Index[CharNumMax];
	int row1Cnt = 0, row2Cnt = 0;
	for (int i = 0; i < (rRects).size(); i++)
	{
		if ((rRects)[i].center.y < center_y)
		{
			row1Index[row1Cnt] = i;
			row1Cnt++;
		}
		else
		{
			row2Index[row2Cnt] = i;
			row2Cnt++;
		}
	}
	vector<RotatedRect> row1Rect(CharNumMax), row2Rect(CharNumMax);
	for (int i =0; i <row1Cnt; i++)
	{
		row1Rect[i] = (rRects)[row1Index[i]];
	}
	for (int i = 0; i <row2Cnt; i++)
	{
		row2Rect[i] = (rRects)[row2Index[i]];
	}
	SortSingleRowRects(row1Rect, rects, row1Cnt);
	SortSingleRowRects(row2Rect, rects + row1Cnt, row2Cnt);
}

/* 对单行字符的包围矩形进行排序，从左至右
* @param rRects 输入的矩形向量
* @param rects 输出的水平矩形向量
* @param num 字符的个数
*/
void MyImgProc::SortSingleRowRects(vector<RotatedRect> &rRects, Rect* rects, int num)
{
	int minIdx = 0, minCenter_x = rRects[0].center.x;
	vector<RotatedRect> rectTemp(1);
	for (int j = 0; j < num - 1; j++)
	{
		minCenter_x = rRects[j].center.x;
		minIdx = j;
		for (int i = j; i < num; i++)
		{
			if (minCenter_x>rRects[i].center.x)
			{
				minCenter_x = rRects[i].center.x;
				minIdx = i;
			}
		}
		rectTemp[0] = rRects[j];
		rRects[j] = rRects[minIdx];
		rRects[minIdx] = rectTemp[0];
	}
	for (int i = 0; i < num; i++)
	{
		int topLeft_x = (rRects)[i].center.x - (rRects)[i].size.width / 2;
		int topLeft_y = (rRects)[i].center.y - (rRects)[i].size.height / 2;
		Rect rect_(topLeft_x, topLeft_y, (rRects)[i].size.width, (rRects)[i].size.height);
		*(rects++) = rect_;
	}
}

//参考博客：https://www.cnblogs.com/willwu/p/6133696.html
/* 旋转图像（保留原图像所有内容）
* @param src 输入图像
* @param dst 输出图像
* @param angle 旋转角度
*/
void MyImgProc::RotateImage(const Mat &src, Mat &dst, float angle)
{
	cv::Point2f center(src.cols / 2, src.rows / 2);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1);
	cv::Rect bbox = cv::RotatedRect(center, src.size(), angle).boundingRect();

	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

	cv::warpAffine(src, dst, rot, bbox.size(), 1, 0, 255);
}

/* 计算旋转矩形的旋转角度
* @param rRects 输入矩形向量
* @return 旋转角度（单位：°）
*/
float MyImgProc::CalculateAngle(const vector<RotatedRect> &rRects)
{
	float angle = rRects[0].angle;
	if (0 < abs(angle) && abs(angle) <= 45)  //逆时针
		angle = angle;
	else if (45 < abs(angle) && abs(angle) < 90) //顺时针
		angle = 90 - abs(angle);
	return angle;
}

/* 识别单个字符， ANN
* @param src 含有字符的图像
* @param charIndex 输出字符所在类别的索引
* @param confidence 输出预测的置信度
* @param model ANN模型
* @param resizeWidth 图像放缩尺寸
* @param resizeHeight 图像放缩尺寸
*/
void MyImgProc::SingleCharRecoANN(const Mat &src, int &charIndex, float &confidence, Ptr<ANN_MLP> &model, int resizeWidth, int resizeHeight)
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

/* 识别多个字符， ANN
* @param src 含有多个字符的整幅图像
* @param charIndexs 输出字符所在类别的索引
* @param confidences 输出预测的置信度
* @param rRects 包围单个字符的矩形的数组
* @param charNum 字符总个数
* @param modelPath ANN模型所在路径
* @param resizeWidth 图像放缩尺寸
* @param resizeHeight 图像放缩尺寸
*/
void MyImgProc::MultiCharRecoANN(const Mat &src, int* charIndexs, float* confidences, Rect* rects, int charNum, string modelPath, int resizeWidth, int resizeHeight)
{
	long t1 = GetTickCount();
	Ptr<ANN_MLP> annModel = StatModel::load<ANN_MLP>(modelPath);
	long t2 = GetTickCount();
	cout << "加载模型时间：" << (t2 - t1) << "ms" << endl;
	for (int i = 0; i < charNum; i++)
	{
		Mat singleCharImg = src(rects[i]);
		SingleCharRecoANN(singleCharImg, *charIndexs, *confidences, annModel), ++charIndexs, ++confidences;
		if (0)
		{
			cv::namedWindow("result", CV_WINDOW_NORMAL);
			imshow("result", singleCharImg);
			waitKey(0);
			destroyWindow("result");
		}
	}
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
void MyImgProc::SingleCharRecoSVM(const Mat &src, Mat &sampleFeatureMat, HOGDescriptor &hog, int &prediction, Ptr<SVM> &model, int resizeWidth, int resizeHeight)
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

/* 识别多个字符， SVM
* @param src 含有所有字符的整幅图像
* @param predictions 预测字符类别
* @param rects 字符包围矩形的矩阵
* @param charNum 字符个数
* @param modelPath 模型所在路径
* @param resizeWidth 图像放缩尺寸
* @param resizeHeight 图像放缩尺寸
*/
void MyImgProc::MultiCharRecoSVM(const Mat &src, int* predictions, Rect* rects, int charNum, string modelPath, int resizeWidth, int resizeHeight)
{
	Mat sampleFeatureMat;
	//检测窗口(64,128),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9
	HOGDescriptor hog(Size(8, 16), Size(8, 8), Size(4, 4), Size(4, 4), 5);
	sampleFeatureMat = Mat::zeros(1, HogFeatureDim, CV_32FC1);

	long t1 = GetTickCount();
	Ptr<SVM> svmModel = StatModel::load<SVM>(modelPath);
	long t2 = GetTickCount();
	cout << "加载模型时间：" << (t2 - t1) << "ms" << endl;
	for (int i = 0; i < charNum; i++)
	{
		Mat singleCharImg = src(rects[i]);
		
		SingleCharRecoSVM(singleCharImg, sampleFeatureMat, hog, *predictions, svmModel), ++predictions;
		
		if (0)
		{
			cv::namedWindow("result", CV_WINDOW_NORMAL);
			imshow("result", singleCharImg);
			waitKey(0);
			destroyWindow("result");
		}
	}
}
