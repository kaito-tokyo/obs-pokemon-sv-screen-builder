#pragma once

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

class OpponentRankExtractor {
public:
	const cv::Rect lineRect = {544, 831, 834, 36};
	const std::string language = "ja";
	const double matchThreshold = 5;

	cv::Rect extract(const cv::Mat &screenBinary) const;

private:
	static const std::map<std::string, std::pair<int, int>> PAREN_MAP;
	static const std::vector<cv::Mat> PAREN_TEMPLATES;
};
