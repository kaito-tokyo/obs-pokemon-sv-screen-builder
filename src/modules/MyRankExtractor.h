#pragma once

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

class MyRankExtractor {
public:
	const cv::Rect lineRect = {1206, 419, 1615, 474};

	cv::Rect extract(const cv::Mat &screenBinary) const;

private:
	static const std::vector<cv::Mat> ICON_TEMPLATES;
};
