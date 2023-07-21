#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class MyPokemonCropper {
public:
	const std::vector<cv::Rect> rects = {
		{182, 147, 529, 107}, {182, 263, 529, 107},
		{182, 379, 529, 107}, {182, 496, 529, 107},
		{182, 612, 529, 107}, {182, 727, 529, 107},
	};
	const uchar backgroundValueThreshold = 128;
	const cv::Point backgroundPoint = {0, 0};

	std::vector<cv::Mat> crop(const cv::Mat &input) const
	{
		std::vector<cv::Mat> output(rects.size());
		for (size_t i = 0; i < rects.size(); i++) {
			output[i] = input(rects[i]);
		}
		return output;
	}

	std::vector<bool> getShouldUpdate(const cv::Mat &inputHsv) const
	{
		std::vector<bool> output(rects.size());
		for (size_t i = 0; i < rects.size(); i++) {
			output[i] = inputHsv(rects[i]).at<cv::Vec3b>(
					    backgroundPoint)[2] <
				    backgroundValueThreshold;
		}
		return output;
	}
};
