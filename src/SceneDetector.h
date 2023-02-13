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
		SCENE_BLACK_TRANSITION
	};

	const HistClassifier &classifierLobbyMySelect;
	const HistClassifier &classifierLobbyOpponentSelect;
	const HistClassifier &classifierBlackTransition;

	SceneDetector(const HistClassifier &classifierLobbyMySelect,
		      const HistClassifier &classifierLobbyOpponentSelect,
		      const HistClassifier &classifierBlackTransition)
		: classifierLobbyMySelect(classifierLobbyMySelect),
		  classifierLobbyOpponentSelect(classifierLobbyOpponentSelect),
		  classifierBlackTransition(classifierBlackTransition)
	{
	}

	Scene detectScene(const cv::Mat &screenHSV);
	bool isSelectPokemonScreen(const cv::Mat &screenHSV);
	bool isBlackTransition(const cv::Mat &screenHSV);
	void calcHistHue(const cv::Mat &areaHSV, cv::Mat &hist, int channel,
			 int nBins);
	bool predictByHueHist(const cv::Mat &screenHSV,
			      const HistClassifier &classifier);
};
