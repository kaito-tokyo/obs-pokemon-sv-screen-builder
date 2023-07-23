#pragma once

#include <opencv2/opencv.hpp>

#include "HistClassifier.hpp"
#include "TemplateClassifier.hpp"
#include "GameplayScene.hpp"

class SceneDetector {
public:
	SceneDetector(const TemplateClassifier &_lobbyRankShown,
		      const HistClassifier &_lobbyMySelect_,
		      const HistClassifier &_lobbyOpponentSelect,
		      const HistClassifier &_blackTransition)
		: lobbyRankShown(_lobbyRankShown),

		  lobbyMySelect(_lobbyMySelect_),
		  lobbyOpponentSelect(_lobbyOpponentSelect),
		  blackTransition(_blackTransition)
	{
	}

	GameplayScene detectScene(const cv::Mat &gameplayHSV,
				  const cv::Mat &gameplayGray) const
	{
		if (isSelectPokemonScreen(gameplayHSV)) {
			return GameplayScene::SELECT_POKEMON;
		} else if (isBlackTransition(gameplayHSV)) {
			return GameplayScene::BLACK_TRANSITION;
		} else if (isOpponentRankShown(gameplayGray)) {
			return GameplayScene::RANK_SHOWN;
		} else {
			return GameplayScene::UNKNOWN;
		}
	}

	bool isSelectPokemonScreen(const cv::Mat &gameplayHSV) const
	{
		return lobbyMySelect(gameplayHSV) &&
		       lobbyOpponentSelect(gameplayHSV);
	}

	bool isBlackTransition(const cv::Mat &gameplayHSV) const
	{
		return blackTransition(gameplayHSV);
	}

	bool isOpponentRankShown(const cv::Mat &gameplayGray) const
	{
		return lobbyRankShown(gameplayGray);
	}

private:
	const TemplateClassifier &lobbyRankShown;
	const HistClassifier &lobbyMySelect;
	const HistClassifier &lobbyOpponentSelect;
	const HistClassifier &blackTransition;
};
