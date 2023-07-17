#include "SceneDetector.h"
#include <obs.h>

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

bool SceneDetector::predictByHueHist(const cv::Mat &screenHSV,
				     const HistClassifier &classifier)
{
	const double xScale = screenHSV.cols / 1920.0,
		     yScale = screenHSV.rows / 1080.0;
	const cv::Range rowRange(
		static_cast<int>(classifier.rangeRow[0] * yScale),
		static_cast<int>(classifier.rangeRow[1] * yScale)),
		colRange(static_cast<int>(classifier.rangeCol[0] * xScale),
			 static_cast<int>(classifier.rangeCol[1] * xScale));

	const cv::Mat areaHSV = screenHSV(rowRange, colRange);
	cv::Mat hist;
	calcHistHue(areaHSV, hist, classifier.histChannel, classifier.histBins);

	double maxVal;
	cv::Point maxIdx;
	cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, &maxIdx);
	return maxVal > static_cast<double>(areaHSV.total()) *
				classifier.histRatio &&
	       maxIdx.y == classifier.histMaxIndex;
}

cv::Mat SceneDetector::generateTextBinaryScreen(const cv::Mat &screenBGRA)
{
	cv::Mat screenTextBinary;
	cv::cvtColor(screenBGRA, screenTextBinary, cv::COLOR_BGRA2GRAY);
	return screenTextBinary;
}

#include <iostream>
bool SceneDetector::isOpponentRankShown(const cv::Mat &screenTextBinary)
{
	cv::Range colRange{542, 661}, rowRange{894, 931};
	if (colRange.end > screenTextBinary.cols ||
	    rowRange.end > screenTextBinary.rows)
		return false;
	cv::Mat image = screenTextBinary(rowRange, colRange);
	for (size_t i = 0; i < TEXT_TEMPLATES.size(); i++) {
		const cv::Mat &resultImage = image ^ TEXT_TEMPLATES[i];
		const auto expected = cv::sum(TEXT_TEMPLATES[i] / 255);
		const auto actual = cv::sum(resultImage / 255);
		blog(LOG_INFO, "isOp %f:%f", actual[0], expected[0]);
		if (actual[0] < 50) {
			return true;
		}
	}
	return false;
}
