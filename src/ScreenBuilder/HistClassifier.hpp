#pragma once

#include <string>

#include <opencv2/opencv.hpp>

#include "plugin-support.h"
#include <obs.h>

struct HistClassifier {
public:
	HistClassifier(std::string _name, cv::Rect _rect, int _channel, int _nBins, int _maxIndex,
		       double _ratio)
		: name(_name),
		  rect(_rect),
		  channel(_channel),
		  nBins(_nBins),
		  maxIndex(_maxIndex),
		  ratio(_ratio)
	{
	}

	bool operator()(const cv::Mat &gameplayHSV) const
	{
		assert(gameplayHSV.cols == 1920 && gameplayHSV.rows == 1080);
		const cv::Mat areaHSV = gameplayHSV(rect);

		cv::Mat hist;
		const int channels[]{channel};
		const int histSize[]{nBins};
		const float hranges[] = {0, channel == 0 ? 180.0f : 256.0f};
		const float *ranges[]{hranges};
		cv::calcHist(&areaHSV, 1, channels, {}, hist, 1, histSize,
			     ranges);

		double maxVal;
		cv::Point maxIdx;
		cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, &maxIdx);
		return maxVal > static_cast<double>(areaHSV.total()) * ratio &&
		       maxIdx.y == maxIndex;
	}

private:
    const std::string name;
	const cv::Rect rect;
	const int channel;
	const int nBins;
	const int maxIndex;
	const double ratio;
};
