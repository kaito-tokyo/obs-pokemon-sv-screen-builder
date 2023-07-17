#pragma once

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

class OpponentRankExtractor {
private:
	static const std::map<std::string, std::pair<int, int>> PAREN_MAP;
	static const std::vector<cv::Mat> PAREN_TEMPLATES;
};
