#pragma once

#include <functional>
#include <string>

#include <opencv2/opencv.hpp>

#include "TextRecognizer/TextRecognizer.h"

class MyPokemonNameRecognizer {
public:
	MyPokemonNameRecognizer(cv::Rect _rect, double _thresh)
		: rect(_rect), thresh(_thresh)
	{
	}

	void operator()(const cv::Mat &myPokemonImageGray,
			std::function<void(std::string)> callback) const
	{
		const auto nameImageGray = myPokemonImageGray(rect);
		cv::Mat nameImageBinary, paddedNameImageBianry;
		cv::threshold(nameImageGray, nameImageBinary, thresh, 255,
			      cv::THRESH_BINARY_INV);
		recognizeText(nameImageBinary, callback);
	}

private:
	cv::Rect rect;
	double thresh;
};
