#if !defined MYIMGPROC_H
#define MYIMGPROC_H

class MyImgProc
{
public:
	MyImgProc();
	~MyImgProc();

	void Emphasize(cv::Mat src, cv::Mat* dst, int maskwidth, int maskheight, float factor);
	void Morphology(cv::Mat src, cv::Mat* dst);
	void FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rects, int areaMin, int areaMax, bool externalFlag=false, bool horizontalRectFlag=false);
	void DrawRects(cv::Mat* src, std::vector<cv::RotatedRect> rects, bool showFlag=false, cv::Scalar color = cv::Scalar(0, 255, 0));
	void SortMultiRowRects(std::vector<cv::RotatedRect> rotatedRects, cv::Rect* rect, int row=2);
	void SortSingleRowRects(std::vector<cv::RotatedRect> rotatedRects, cv::Rect* rect);

	float CalculateAngle(std::vector<cv::RotatedRect> rect);
	char SingleCharReco(cv::Mat src, std::string filePath, std::string totalChar="0123456789", int resizeWidth = 8, int resizeHeight = 16);
};

#endif