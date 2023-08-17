#pragma once

#include <opencv2/opencv.hpp>

#include "pokemon-detector-sv.h"

class ResultRecognizer {
public:
	int nBins;
	double loseRatio;
	int loseMaxIndex;
	double winRatio;
	int winMaxIndex;

	ResultRecognizer(int nBins, int loseMaxIndex, double loseRatio,
			 int winMaxIndex, double winRatio)
		: nBins(nBins),
		  loseRatio(loseRatio),
		  loseMaxIndex(loseMaxIndex),
		  winRatio(winRatio),
		  winMaxIndex(winMaxIndex)
	{
	}

	std::string recognizeResult(const cv::Mat &imageHSV)
	{
		const cv::Mat areaHSV = areaBGR;
		cv::Mat hist;
		calcHistHue(areaHSV, hist);

		double maxVal;
		cv::Point maxIdx;
		cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, &maxIdx);
		if (maxVal > areaHSV.total() * loseRatio &&
		    maxIdx.y == loseMaxIndex) {
			return "LOSE";
		} else if (maxVal > areaHSV.total() * winRatio &&
			   maxIdx.y == winMaxIndex) {
			return "WIN";
		} else {
			return "UNKNOWN";
		}
	}

private:
	void calcHistHue(const cv::Mat &areaHSV, cv::Mat &hist)
	{
		const int channels[]{0};
		const int histSize[]{nBins};
		const float hranges[] = {0, 180};
		const float *ranges[]{hranges};
		cv::calcHist(&areaHSV, 1, channels, cv::Mat(), hist, 1,
			     histSize, ranges);
	}
};
