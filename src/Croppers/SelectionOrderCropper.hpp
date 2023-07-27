#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class SelectionOrderCropper {
public:
	SelectionOrderCropper(std::vector<cv::Rect> _rects) : rects(_rects) {}

	std::vector<cv::Mat> crop(const cv::Mat &input) const
	{
		std::vector<cv::Mat> output(rects.size());
		for (size_t i = 0; i < rects.size(); i++) {
			output[i] = input(rects[i]);
		}
		return output;
	}

private:
	const std::vector<cv::Rect> rects;
};
