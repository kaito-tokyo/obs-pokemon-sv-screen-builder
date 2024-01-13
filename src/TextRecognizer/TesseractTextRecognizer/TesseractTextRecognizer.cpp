#include <string>

#include <opencv2/opencv.hpp>

#include <obs.h>

void recognizeText(const cv::Mat imageBinary,
		   std::function<void(std::string)> callback)
{
	UNUSED_PARAMETER(imageBinary);
	callback("N/A");
}
