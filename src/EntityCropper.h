#pragma once

#include <array>
#include <vector>

#include <opencv2/opencv.hpp>

class EntityCropper {
public:
	std::vector<cv::Mat> imagesBGR;
	std::vector<cv::Mat> imagesBGRA;
	std::vector<cv::Mat> masks;
	const std::array<int, 2> rangeCol;
	const std::vector<std::array<int, 2>> rangesRow;
	cv::Point seedPoint;

	EntityCropper(const std::array<int, 2> rangeCol_,
		      const std::vector<std::array<int, 2>> rangesRow_)
		: imagesBGR(rangesRow_.size()),
		  imagesBGRA(rangesRow_.size()),
		  masks(rangesRow_.size()),
		  rangeCol(rangeCol_),
		  rangesRow(rangesRow_),
		  seedPoint(0, 0)
	{
	}
	void crop(const cv::Mat &screenBGRA);
	void generateMask(void);
};
