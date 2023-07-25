#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class SelectionRecognizer {
public:
	int recognizeSelection(const cv::Mat &imageBGR) const;

private:
	static const std::vector<int> SELECTION_INDEX;
	static const std::vector<cv::Mat> SELECTION_TEMPLATES;
	static const int NORMALIZED_HEIGHT;
	static const int DESCRIPTOR_SIZE;
	static const int BINARY_THRESHOLD;
};
