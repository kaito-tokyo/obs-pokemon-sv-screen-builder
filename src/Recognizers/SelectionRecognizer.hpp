#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class SelectionRecognizer {
public:
	SelectionRecognizer(int _blueThreshold, int _binaryThreshold,
			    double ratio, const std::vector<int> &_indices,
			    const std::vector<int> &_cols,
			    const std::vector<std::vector<uchar>> &_data);

	int operator()(const cv::Mat &imageBGR, const cv::Mat &imageGray) const;

private:
	const int blueThreshold;
	const int binaryThreshold;
	const double ratio;
	const std::vector<int> indices;
	const std::vector<int> cols;
	const std::vector<std::vector<uchar>> data;
	const std::vector<cv::Mat> templates;
};
