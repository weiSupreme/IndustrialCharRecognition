// CharRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	Mat SrcImg = imread("D:/ÊµÏ°/Í¼Æ¬/pic/2017.08.25/C1-08251718/C1_0.bmp", 0);
	if (SrcImg.empty())
	{
		std::cout << "´ò¿ªÍ¼Æ¬Ê§°Ü,Çë¼ì²é" << std::endl;
		return -1;
	}
	Mat DstImg = SrcImg.clone();
	MyImgProc *mip = new MyImgProc();

	mip->Emphasize(SrcImg, DstImg, 7, 7, 5);
	threshold(SrcImg, DstImg, 180, 255, 1);

	imshow("img", DstImg);
	//imwrite("D:/ÊµÏ°/Í¼Æ¬/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	waitKey(0);

	return 0;
}

