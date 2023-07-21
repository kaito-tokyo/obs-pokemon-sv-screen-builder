#pragma once

#include "ScreenState.hpp"
#include "modules/SceneDetector.h"
#include "ActionHandler.hpp"
#include "constants.h"

class Automaton {
public:
	ActionHandler actionHandler;

	SceneDetector sceneDetector;

	Automaton(ActionHandler _actionHandler, SceneDetector _sceneDetector)
		: actionHandler(_actionHandler), sceneDetector(_sceneDetector)
	{
	}

	void operator()(const cv::Mat &gameplayBGRA);

	ScreenState compute(SceneDetector::Scene scene);
	ScreenState computeUnknown(SceneDetector::Scene scene);
	ScreenState computeEnteringRankShown(void);
	ScreenState computeRankShown(SceneDetector::Scene scene);
	ScreenState computeEnteringSelectPokemon(void);
	ScreenState computeSelectPokemon(SceneDetector::Scene scene);
	ScreenState computeLeavingSelectPokemon(SceneDetector::Scene scene);
	ScreenState computeEnteringConfirmPokemon(SceneDetector::Scene scene);
	ScreenState computeConfirmPokemon(SceneDetector::Scene scene);
	ScreenState computeEnteringMatch(SceneDetector::Scene scene);
	ScreenState computeMatch(SceneDetector::Scene scene);
	ScreenState computeEnteringResult(void);
	ScreenState computeResult(SceneDetector::Scene scene);

private:
	SceneDetector::Scene prevScene = SceneDetector::SCENE_UNDEFINED;
	ScreenState state = ScreenState::UNKNOWN;
	cv::Mat gameplayBGRA;
	cv::Mat gameplayBGR;
	cv::Mat gameplayHSV;
	cv::Mat gameplayGray;
	uint64_t lastStateChangeNs = 0;
	uint64_t elapsedNsFromLastStateChange = 0;
	std::array<int, N_POKEMONS> mySelectionOrderMap;
	std::array<cv::Mat, N_POKEMONS> myPokemonsBGRA;
};
