#if !defined MYIMGPROC_H
#define MYIMGPROC_H

class MyImgProc
{
public:
	MyImgProc();
	~MyImgProc();

	void Emphasize(cv::Mat src, cv::Mat* dst, int maskwidth, int maskheight, float factor);
	void Morphology(cv::Mat src, cv::Mat* dst);

	void FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rects);
};

#endif