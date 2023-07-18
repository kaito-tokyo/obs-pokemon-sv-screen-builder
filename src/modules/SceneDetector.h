#pragma once

#include <opencv2/opencv.hpp>

struct HistClassifier {
	const int rangeCol[2];
	const int rangeRow[2];
	const int histChannel;
	const int histBins;
	const int histMaxIndex;
	const double histRatio;
};

class SceneDetector {
public:
	enum Scene {
		SCENE_UNDEFINED,
		SCENE_SELECT_POKEMON,
		SCENE_BLACK_TRANSITION,
		SCENE_SHOW_RANK,
	};

	const HistClassifier &classifierLobbyMySelect;
	const HistClassifier &classifierLobbyOpponentSelect;
	const HistClassifier &classifierBlackTransition;

	SceneDetector(const HistClassifier &classifierLobbyMySelect_,
		      const HistClassifier &classifierLobbyOpponentSelect_,
		      const HistClassifier &classifierBlackTransition_)
		: classifierLobbyMySelect(classifierLobbyMySelect_),
		  classifierLobbyOpponentSelect(classifierLobbyOpponentSelect_),
		  classifierBlackTransition(classifierBlackTransition_)
	{
	}

	Scene detectScene(const cv::Mat &screenHSV,
			  const cv::Mat &screenBinary) const;
	bool isSelectPokemonScreen(const cv::Mat &screenHSV) const;
	bool isBlackTransition(const cv::Mat &screenHSV) const;
	void calcHistHue(const cv::Mat &areaHSV, cv::Mat &hist, int channel,
			 int nBins) const;
	bool predictByHueHist(const cv::Mat &screenHSV,
			      const HistClassifier &classifier) const;

	cv::Mat generateTextBinaryScreen(const cv::Mat &screenBGRA);
	bool isOpponentRankShown(const cv::Mat &screenTextBinary) const;

private:
	static const int TEXT_THRESHOLD;
	static const std::vector<cv::Mat> TEXT_TEMPLATES;
};
