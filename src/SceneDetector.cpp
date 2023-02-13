#include "SceneDetector.h"

SceneDetector::Scene SceneDetector::detectScene(const cv::Mat &screenHSV)
{
	if (isSelectPokemonScreen(screenHSV)) {
		return SCENE_SELECT_POKEMON;
	} else if (isBlackTransition(screenHSV)) {
		return SCENE_BLACK_TRANSITION;
	} else {
		return SCENE_UNDEFINED;
	}
}

bool SceneDetector::isSelectPokemonScreen(const cv::Mat &screenHSV)
{
	return predictByHueHist(screenHSV, classifierLobbyMySelect) &&
	       predictByHueHist(screenHSV, classifierLobbyOpponentSelect);
}

bool SceneDetector::isBlackTransition(const cv::Mat &screenHSV)
{
	return predictByHueHist(screenHSV, classifierBlackTransition);
}

void SceneDetector::calcHistHue(const cv::Mat &areaHSV, cv::Mat &hist,
				int channel, int nBins)
{
	const int channels[]{channel};
	const int histSize[]{nBins};
	const float hranges[] = {0, channel == 0 ? 180.0f : 256.0f};
	const float *ranges[]{hranges};
	cv::calcHist(&areaHSV, 1, channels, cv::Mat(), hist, 1, histSize,
		     ranges);
}

#include <iostream>
bool SceneDetector::predictByHueHist(const cv::Mat &screenHSV,
				     const HistClassifier &classifier)
{
	const double xScale = screenHSV.cols / 1920.0,
		     yScale = screenHSV.rows / 1080.0;
	const cv::Range rowRange(classifier.rangeRow[0] * yScale,
				 classifier.rangeRow[1] * yScale),
		colRange(classifier.rangeCol[0] * xScale,
			 classifier.rangeCol[1] * xScale);

	const cv::Mat areaHSV = screenHSV(rowRange, colRange);
	cv::Mat hist;
	calcHistHue(areaHSV, hist, classifier.histChannel, classifier.histBins);

	double maxVal;
	cv::Point maxIdx;
	cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, &maxIdx);
	return maxVal > areaHSV.total() * classifier.histRatio &&
	       maxIdx.y == classifier.histMaxIndex;
}
