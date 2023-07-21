#pragma once

#include <string>
#include <opencv2/opencv.hpp>

#ifdef __APPLE__
std::string recognizeText(const cv::Mat &imageBGR);
#elif _WIN32
std::string recognizeTextImpl(const unsigned char *data, int width, int height);

static std::string recognizeText(const cv::Mat &imageBGRA)
{
	cv::Mat padImage;
	cv::copyMakeBorder(imageBGRA, padImage, 200, 200, 200, 200,
			   cv::BORDER_CONSTANT, cv::Scalar(255));
	return recognizeTextImpl(padImage.data, padImage.cols, padImage.rows);
}
#else
static std::string recognizeText(const cv::Mat &imageBGRA)
{
	UNUSED_PARAMETER(imageBGRA);
	return std::string("N/A");
}
#endif
