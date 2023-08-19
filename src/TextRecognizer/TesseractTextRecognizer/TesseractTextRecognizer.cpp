#include <string>

#include <opencv2/opencv.hpp>

#include <obs.h>

std::string recognizeText(const cv::Mat &imageBinary,
			  std::function<void(std::string)> callback)
{
	UNUSED_PARAMETER(imageBinary);
	callback("N/A");
}
