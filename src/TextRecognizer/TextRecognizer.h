#pragma once

#include <functional>
#include <string>

#include <opencv2/opencv.hpp>

std::string recognizeText(const cv::Mat &imageBinary,
			  std::function<void(std::string)> callback);
