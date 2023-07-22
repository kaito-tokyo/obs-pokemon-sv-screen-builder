#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class MyPokemonCropper {
public:
	MyPokemonCropper(std::vector<cv::Rect> _rects,
			 uchar _backgroundValueThreshold,
			 cv::Point _backgroundPoint)
		: rects(_rects),
		  backgroundValueThreshold(_backgroundValueThreshold),
		  backgroundPoint(_backgroundPoint)
	{
	}

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

private:
	const std::vector<cv::Rect> rects;
	const uchar backgroundValueThreshold;
	const cv::Point backgroundPoint;
};
