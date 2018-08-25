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
	void drawRects(cv::Mat* src, std::vector<cv::RotatedRect> rects, bool showFlag=false, cv::Scalar color = cv::Scalar(0, 255, 0));

	float calculateAngle(std::vector<cv::RotatedRect> rect);
};

#endif