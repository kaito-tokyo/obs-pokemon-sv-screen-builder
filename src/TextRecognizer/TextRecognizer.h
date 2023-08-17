#pragma once

#include <string>
#include <opencv2/opencv.hpp>

#if defined(__APPLE__) || defined(_WIN32)
std::string recognizeText(const cv::Mat &imageBGR);
#else
static std::string recognizeText(const cv::Mat &imageBGRA)
{
	UNUSED_PARAMETER(imageBGRA);
	return std::string("N/A");
}
#endif
