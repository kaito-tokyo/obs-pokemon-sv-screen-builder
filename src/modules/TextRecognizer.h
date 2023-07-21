#pragma once

#include <string>
#include <opencv2/opencv.hpp>

#ifdef __APPLE__
std::string recognizeText(const cv::Mat &imageBGR);
#else
static std::string recognizeText(const cv::Mat &imageBGRA)
{
	UNUSED_PARAMETER(imageBGRA);
	return std::string("N/A");
}
#endif
