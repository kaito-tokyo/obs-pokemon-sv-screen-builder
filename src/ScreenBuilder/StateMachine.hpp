#pragma once

#include "ActionHandler.hpp"
#include "GameplayScene.hpp"
#include "MatchState.hpp"
#include "SceneDetector.hpp"
#include "ScreenState.hpp"

class StateMachine {
public:
	ActionHandler actionHandler;
	SceneDetector sceneDetector;

	StateMachine(ActionHandler _actionHandler, SceneDetector _sceneDetector)
		: actionHandler(_actionHandler),
		  sceneDetector(_sceneDetector)
	{
	}

	void operator()(const cv::Mat &gameplayBGRA);

	ScreenState compute(GameplayScene scene);
	ScreenState computeUnknown(GameplayScene scene);
	ScreenState computeEnteringRankShown(void);
	ScreenState computeRankShown(GameplayScene scene);
	ScreenState computeEnteringSelectPokemon(void);
	ScreenState computeSelectPokemon(GameplayScene scene);
	ScreenState computeLeavingSelectPokemon(GameplayScene scene);
	ScreenState computeEnteringConfirmPokemon(GameplayScene scene);
	ScreenState computeConfirmPokemon(GameplayScene scene);
	ScreenState computeEnteringMatch(GameplayScene scene);
	ScreenState computeMatch(GameplayScene scene);
	ScreenState computeEnteringResult(GameplayScene scene);
	ScreenState computeResult(void);

private:
	GameplayScene prevScene = GameplayScene::UNKNOWN;
	ScreenState state = ScreenState::UNKNOWN;
	cv::Mat gameplayBGRA;
	cv::Mat gameplayBGR;
	cv::Mat gameplayHSV;
	cv::Mat gameplayGray;
	uint64_t lastStateChangeNs = 0;
	uint64_t elapsedNsFromLastStateChange = 0;
	std::vector<int> mySelectionMap = std::vector<int>(6);
	std::vector<cv::Mat> myPokemonImagesBGRA = std::vector<cv::Mat>(6);
	std::vector<cv::Mat> myPokemonImagesGray = std::vector<cv::Mat>(6);
	MatchState matchState;
};
