#pragma once

#ifdef __APPLE__
#include "macos/VisionTextRecognizer.h"
#else
std::string recognizeText(const cv::Mat &imageBGRA)
{
	return std::string("bbbb");
}
#endif
