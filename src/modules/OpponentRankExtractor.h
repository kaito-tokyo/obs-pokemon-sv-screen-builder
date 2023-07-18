#pragma once

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

class OpponentRankExtractor {
public:
	cv::Mat imageBGRA;
	void extract(const cv::Mat &imageTextBinary, const cv::Mat &imageBGRA);

private:
	static const std::map<std::string, std::pair<int, int>> PAREN_MAP;
	static const std::vector<cv::Mat> PAREN_TEMPLATES;
};
