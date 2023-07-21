#include <opencv2/opencv.hpp>
#include <nameof.hpp>

#include <obs.h>

#include "modules/SceneDetector.h"
#include "plugin-support.h"

#include "ActionHandler.hpp"

#include "Automaton.hpp"

void Automaton::operator()(const cv::Mat &_gameplayBGRA)
{
	gameplayBGRA = _gameplayBGRA;
	cv::Mat gameplayBinary;
	cv::cvtColor(gameplayBGRA, gameplayBGR, cv::COLOR_BGRA2BGR);
	cv::cvtColor(gameplayBGR, gameplayHSV, cv::COLOR_BGR2HSV);
	cv::cvtColor(gameplayBGRA, gameplayGray, cv::COLOR_BGRA2GRAY);
	cv::threshold(gameplayGray, gameplayBinary, 200, 255,
		      cv::THRESH_BINARY);
	SceneDetector::Scene currentScene =
		sceneDetector.detectScene(gameplayHSV, gameplayBinary);

	ScreenState nextState = compute(currentScene);
	if (nextState != state) {
		std::string stateString(NAMEOF_ENUM(state));
		std::string nextStateString(NAMEOF_ENUM(nextState));
		obs_log(LOG_INFO, "State: %s to %s", stateString.c_str(),
			nextStateString.c_str());
		state = nextState;
	}
}

ScreenState Automaton::compute(SceneDetector::Scene scene)
{
	switch (state) {
	case ScreenState::UNKNOWN:
		return computeUnknown(scene);
	case ScreenState::ENTERING_RANK_SHOWN:
		return computeEnteringRankShown();
	case ScreenState::RANK_SHOWN:
		return computeRankShown(scene);
	case ScreenState::ENTERING_SELECT_POKEMON:
		return computeEnteringSelectPokemon();
	case ScreenState::SELECT_POKEMON:
		return computeSelectPokemon(scene);
	case ScreenState::LEAVING_SELECT_POKEMON:
		return computeLeavingSelectPokemon(scene);
	case ScreenState::ENTERING_CONFIRM_POKEMON:
		return computeEnteringConfirmPokemon(scene);
	case ScreenState::CONFIRM_POKEMON:
		return computeConfirmPokemon(scene);
	case ScreenState::ENTERING_MATCH:
		return computeEnteringMatch(scene);
	case ScreenState::MATCH:
		return computeMatch(scene);
	case ScreenState::ENTERING_RESULT:
		return computeEnteringResult();
	case ScreenState::RESULT:
		return computeResult(scene);
	}
}

ScreenState Automaton::computeUnknown(SceneDetector::Scene scene)
{
	switch (scene) {
	case SceneDetector::SCENE_RANK_SHOWN:
		return ScreenState::ENTERING_RANK_SHOWN;
	case SceneDetector::SCENE_SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	default:
		return ScreenState::UNKNOWN;
	}
}

ScreenState Automaton::computeEnteringRankShown(void)
{
	actionHandler.handleEnteringRankShown(gameplayGray);
	return ScreenState::RANK_SHOWN;
}

ScreenState Automaton::computeRankShown(SceneDetector::Scene scene)
{
	switch (scene) {
	case SceneDetector::SCENE_SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	default:
		return ScreenState::RANK_SHOWN;
	}
}

ScreenState Automaton::computeEnteringSelectPokemon(void)
{
	if (elapsedNs > 1000000000) {
		return ScreenState::SELECT_POKEMON;
	} else {
		return ScreenState::ENTERING_SELECT_POKEMON;
	}
}

ScreenState Automaton::computeSelectPokemon(SceneDetector::Scene scene)
{
	if (scene != SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::LEAVING_SELECT_POKEMON;
	} else {
		return ScreenState::SELECT_POKEMON;
	}
}

ScreenState Automaton::computeLeavingSelectPokemon(SceneDetector::Scene scene)
{
	switch (scene) {
	case SceneDetector::SCENE_UNDEFINED:
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	case SceneDetector::SCENE_SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	case SceneDetector::SCENE_BLACK_TRANSITION:
		return ScreenState::ENTERING_MATCH;
	case SceneDetector::SCENE_RANK_SHOWN:
		return ScreenState::ENTERING_RANK_SHOWN;
	default:
		return ScreenState::UNKNOWN;
	}
}

ScreenState Automaton::computeEnteringConfirmPokemon(SceneDetector::Scene scene)
{
	if (elapsedNs > 500000000) {
		return ScreenState::CONFIRM_POKEMON;
	} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else {
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	}
}

ScreenState Automaton::computeConfirmPokemon(SceneDetector::Scene scene)
{
	switch (scene) {
	case SceneDetector::SCENE_SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	case SceneDetector::SCENE_BLACK_TRANSITION:
		return ScreenState::ENTERING_MATCH;
	case SceneDetector::SCENE_RANK_SHOWN:
		return ScreenState::ENTERING_RANK_SHOWN;
	default:
		return ScreenState::CONFIRM_POKEMON;
	}
}

ScreenState Automaton::computeEnteringMatch(SceneDetector::Scene scene)
{
	if (prevScene != SceneDetector::SCENE_BLACK_TRANSITION &&
	    scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::MATCH;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::ENTERING_MATCH;
	}
}

ScreenState Automaton::computeMatch(SceneDetector::Scene scene)
{
	if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else if (prevScene != SceneDetector::SCENE_BLACK_TRANSITION &&
		   scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_RESULT;
	} else {
		return ScreenState::MATCH;
	}
}

ScreenState Automaton::computeEnteringResult(void)
{
	return ScreenState::RESULT;
}

ScreenState Automaton::computeResult(SceneDetector::Scene scene)
{
	if (elapsedNs > 2000000000) {
		return ScreenState::UNKNOWN;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::RESULT;
	}
}
