#pragma once

#ifdef __APPLE__
#include "macos/VisionTextRecognizer.h"
#else
std::string recognizeText(const cv::Mat &imageBGRA)
{
	UNUSED_PARAMETER(imageBGRA);
	return std::string("N/A");
}
#endif
