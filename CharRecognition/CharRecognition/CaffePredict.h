#if !defined CAFFE_PREDICT_H
#define CAFFE_PREDICT_H

#include "stdafx.h"
#include<iostream>
#include <string>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2\opencv.hpp>

#include "caffe.hpp"

#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <utility>
#include <vector>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>


class CaffePredict
{
public:
	CaffePredict(const std::string& model_file,
		const std::string& weights_file,


		//const string& mean_file,
		//const string& mean_value,
		const int gpu_id);

	std::vector<float> Predict(const cv::Mat& img);

private:
	//void SetMean(const string& mean_file, const string& mean_value);

	void WrapInputLayer(std::vector<cv::Mat>* input_channels);

	void Preprocess(const cv::Mat& img,
		std::vector<cv::Mat>* input_channels);

private:
	boost::shared_ptr<caffe::Net<float> > net_;
	cv::Size input_geometry_;
	int num_channels_;
	//cv::Mat mean_;

};

#endif