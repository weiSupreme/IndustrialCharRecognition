#if !defined MYIMGPROC_H
#define MYIMGPROC_H

class MyImgProc
{
public:
	MyImgProc();
	~MyImgProc();
	void Emphasize(cv::Mat src, cv::Mat dst, int maskwidth, int maskheight, float factor);
};

#endif