#pragma once

#include <opencv2/opencv.hpp>

#include <obs.h>

#include "plugin-support.h"

class ResultRecognizer {
public:
	ResultRecognizer(int _nBins, int _winMaxIndex, double _winRatio,
			 int _loseMaxIndex, double _loseRatio)
		: nBins(_nBins),
		  winMaxIndex(_winMaxIndex),
		  winRatio(_winRatio),
		  loseMaxIndex(_loseMaxIndex),
		  loseRatio(_loseRatio)
	{
	}

	std::string recognizeResult(const cv::Mat &imageHSV) const
	{
		cv::Mat hist;
		calcHistHue(imageHSV, hist);

		double maxVal;
		cv::Point maxIdx;
		cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, &maxIdx);
		obs_log(LOG_INFO, "ResultRecognizer: %d %f %d",
			imageHSV.total(), maxVal, maxIdx.y);
		if (maxVal > imageHSV.total() * winRatio &&
		    maxIdx.y == winMaxIndex) {
			return "WIN";
		} else if (maxVal > imageHSV.total() * loseRatio &&
			   maxIdx.y == loseMaxIndex) {
			return "LOSE";
		} else {
			return "UNKNOWN";
		}
	}

private:
	const int nBins;
	const int winMaxIndex;
	const double winRatio;
	const int loseMaxIndex;
	const double loseRatio;

	void calcHistHue(const cv::Mat &imageHSV, cv::Mat &hist) const
	{
		const int channels[]{0};
		const int histSize[]{nBins};
		const float hranges[] = {0, 180};
		const float *ranges[]{hranges};
		cv::calcHist(&imageHSV, 1, channels, {}, hist, 1, histSize,
			     ranges);
	}
};
