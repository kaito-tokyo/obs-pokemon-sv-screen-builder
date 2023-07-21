#pragma once

#include <string>
#include <opencv2/opencv.hpp>

#ifdef __APPLE__
std::string recognizeText(const cv::Mat &imageBGR);
#elif _WIN32
std::string recognizeTextImpl(const unsigned char *data, int width, int height);

static std::string recognizeText(const cv::Mat &imageBGRA)
{
	return recognizeTextImpl(imageBGRA.data, imageBGRA.cols, imageBGRA.rows);
}
#else
static std::string recognizeText(const cv::Mat &imageBGRA)
{
	UNUSED_PARAMETER(imageBGRA);
	return std::string("N/A");
}
#endif
