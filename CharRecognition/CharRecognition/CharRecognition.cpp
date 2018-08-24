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
	Mat srcImg = imread("D:/ÊµÏ°/Í¼Æ¬/pic/2017.08.25/C1-08251718/C1_12 (2).bmp", 0);
	if (srcImg.empty())
	{
		std::cout << "´ò¿ªÍ¼Æ¬Ê§°Ü,Çë¼ì²é" << std::endl;
		return -1;
	}
	
	MyImgProc *mip = new MyImgProc();

	Mat emphasizeImg;
	mip->Emphasize(srcImg, &emphasizeImg, 7, 7, 5);
	Mat binaryImg;
	threshold(emphasizeImg, binaryImg, 190, 255, 1);

	Mat morphologyImg;
	mip->Morphology(binaryImg, &morphologyImg);

	vector<RotatedRect> rects;
	mip->FindTextRegion(morphologyImg, &rects);

	//»­³öÂÖÀª
	for each (RotatedRect rect in rects)
	{
		Point2f P[4];
		rect.points(P);
		for (int j = 0; j <= 3; j++)
		{
			line(srcImg, P[j], P[(j + 1) % 4], Scalar(0, 255, 0), 2);
		}
	}

	imshow("img", srcImg);
	//imwrite("D:/ÊµÏ°/Í¼Æ¬/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	waitKey(0);

	return 0;
}

