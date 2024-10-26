#include <opencv2/opencv.hpp>

#include <obs.h>
#include <util/platform.h>

#include "plugin-support.h"

#include "ActionHandler.hpp"

#include "StateMachine.hpp"

void StateMachine::operator()(const cv::Mat &_gameplayBGRA)
{
	gameplayBGRA = _gameplayBGRA;
	cv::Mat gameplayBinary;
	cv::cvtColor(gameplayBGRA, gameplayBGR, cv::COLOR_BGRA2BGR);
	cv::cvtColor(gameplayBGR, gameplayHSV, cv::COLOR_BGR2HSV);
	cv::cvtColor(gameplayBGRA, gameplayGray, cv::COLOR_BGRA2GRAY);
	GameplayScene currentScene =
		sceneDetector.detectScene(gameplayHSV, gameplayGray);

	uint64_t nowNs = os_gettime_ns();
	elapsedNsFromLastStateChange = nowNs - lastStateChangeNs;
	ScreenState nextState = compute(currentScene);
	prevScene = currentScene;
	if (nextState != state) {
		std::string stateString(screenStateToString(state));
		std::string nextStateString(screenStateToString(nextState));
		obs_log(LOG_INFO, "State: %s to %s", stateString.c_str(),
			nextStateString.c_str());
		state = nextState;
		lastStateChangeNs = nowNs;
	}
}

ScreenState StateMachine::compute(GameplayScene scene)
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
		return computeEnteringResult(scene);
	case ScreenState::RESULT:
		return computeResult();
	default:
		return ScreenState::UNKNOWN;
	}
}

ScreenState StateMachine::computeUnknown(GameplayScene scene)
{
	switch (scene) {
	case GameplayScene::RANK_SHOWN:
		return ScreenState::ENTERING_RANK_SHOWN;
	case GameplayScene::SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	default:
		return ScreenState::UNKNOWN;
	}
}

ScreenState StateMachine::computeEnteringRankShown(void)
{
	actionHandler.handleEnteringRankShown(gameplayGray, matchState);
	return ScreenState::RANK_SHOWN;
}

ScreenState StateMachine::computeRankShown(GameplayScene scene)
{
	switch (scene) {
	case GameplayScene::SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	default:
		return ScreenState::RANK_SHOWN;
	}
}

ScreenState StateMachine::computeEnteringSelectPokemon(void)
{
	bool canEnterToSelectPokemon = elapsedNsFromLastStateChange >
				       2000000000;
	actionHandler.handleEnteringSelectPokemon(gameplayBGRA, gameplayBGR,
						  canEnterToSelectPokemon,
						  mySelectionMap, matchState);
	if (canEnterToSelectPokemon) {
		return ScreenState::SELECT_POKEMON;
	} else {
		return ScreenState::ENTERING_SELECT_POKEMON;
	}
}

ScreenState StateMachine::computeSelectPokemon(GameplayScene scene)
{
	actionHandler.handleSelectPokemon(gameplayBGRA, gameplayBGR,
					  gameplayHSV, gameplayGray,
					  mySelectionMap, myPokemonImagesBGRA,
					  myPokemonImagesGray, matchState);
	if (scene != GameplayScene::SELECT_POKEMON) {
		return ScreenState::LEAVING_SELECT_POKEMON;
	} else {
		return ScreenState::SELECT_POKEMON;
	}
}

ScreenState StateMachine::computeLeavingSelectPokemon(GameplayScene scene)
{
	switch (scene) {
	case GameplayScene::UNKNOWN:
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	case GameplayScene::SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	case GameplayScene::BLACK_TRANSITION:
		return ScreenState::ENTERING_MATCH;
	case GameplayScene::RANK_SHOWN:
		return ScreenState::ENTERING_RANK_SHOWN;
	default:
		return ScreenState::UNKNOWN;
	}
}

ScreenState StateMachine::computeEnteringConfirmPokemon(GameplayScene scene)
{
	if (elapsedNsFromLastStateChange > 500000000) {
		return ScreenState::CONFIRM_POKEMON;
	} else if (scene == GameplayScene::BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else {
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	}
}

ScreenState StateMachine::computeConfirmPokemon(GameplayScene scene)
{
	switch (scene) {
	case GameplayScene::SELECT_POKEMON:
		return ScreenState::ENTERING_SELECT_POKEMON;
	case GameplayScene::BLACK_TRANSITION:
		return ScreenState::ENTERING_MATCH;
	case GameplayScene::RANK_SHOWN:
		return ScreenState::ENTERING_RANK_SHOWN;
	default:
		return ScreenState::CONFIRM_POKEMON;
	}
}

ScreenState StateMachine::computeEnteringMatch(GameplayScene scene)
{
	bool canEnterToMatch = prevScene != GameplayScene::BLACK_TRANSITION &&
			       scene == GameplayScene::BLACK_TRANSITION;
	actionHandler.handleEnteringMatch(canEnterToMatch);
	if (canEnterToMatch) {
		return ScreenState::MATCH;
	} else if (scene == GameplayScene::SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == GameplayScene::RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::ENTERING_MATCH;
	}
}

ScreenState StateMachine::computeMatch(GameplayScene scene)
{
	if (scene == GameplayScene::SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == GameplayScene::RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else if (prevScene != GameplayScene::BLACK_TRANSITION &&
		   scene == GameplayScene::BLACK_TRANSITION) {
		return ScreenState::ENTERING_RESULT;
	} else {
		return ScreenState::MATCH;
	}
}

ScreenState StateMachine::computeEnteringResult(GameplayScene scene)
{
	if (elapsedNsFromLastStateChange > 2000000000) {
		return ScreenState::RESULT;
	} else if (scene == GameplayScene::SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == GameplayScene::RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::ENTERING_RESULT;
	}
}

ScreenState StateMachine::computeResult(void)
{
	actionHandler.handleResult(gameplayHSV, matchState);
	return ScreenState::UNKNOWN;
}
