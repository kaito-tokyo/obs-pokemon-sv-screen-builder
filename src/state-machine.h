#pragma once

#include <map>

#include <nlohmann/json.hpp>

#include "modules/SceneDetector.h"
#include "modules/OpponentRankExtractor.h"
#include "modules/TextRecognizer.h"
#include "modules/PokemonRecognizer.h"

#include "constants.h"
#include "renderers.h"
#include "obs-browser-api.h"

enum class ScreenState {
	UNKNOWN,
	ENTERING_RANK_SHOWN,
	RANK_SHOWN,
	ENTERING_SELECT_POKEMON,
	SELECT_POKEMON,
	LEAVING_SELECT_POKEMON,
	ENTERING_CONFIRM_POKEMON,
	CONFIRM_POKEMON,
	ENTERING_MATCH,
	MATCH,
	ENTERING_RESULT,
	RESULT,
};

const std::map<ScreenState, const char *> ScreenStateNames = {
	{ScreenState::UNKNOWN, "UNKNOWN"},
	{ScreenState::ENTERING_RANK_SHOWN, "ENTERING_RANK_SHOWN"},
	{ScreenState::RANK_SHOWN, "RANK_SHOWN"},
	{ScreenState::ENTERING_SELECT_POKEMON, "ENTERING_SELECT_POKEMON"},
	{ScreenState::SELECT_POKEMON, "SELECT_POKEMON"},
	{ScreenState::LEAVING_SELECT_POKEMON, "LEAVING_SELECT_POKEMON"},
	{ScreenState::ENTERING_CONFIRM_POKEMON, "ENTERING_CONFIRM_POKEMON"},
	{ScreenState::CONFIRM_POKEMON, "CONFIRM_POKEMON"},
	{ScreenState::ENTERING_MATCH, "ENTERING_MATCH"},
	{ScreenState::MATCH, "MATCH"},
	{ScreenState::ENTERING_RESULT, "ENTERING_RESULT"},
	{ScreenState::RESULT, "RESULT"},
};

const char EVENT_NAME_OPPONENT_RANK_SHOWN[] =
	"obsPokemonSvScreenBuilderOpponentRankShown";

static ScreenState handleUnknown(SceneDetector::Scene scene)
{
	if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else {
		return ScreenState::UNKNOWN;
	}
}

static ScreenState
handleEnteringRankShown(const OpponentRankExtractor &opponentRankExtractor,
			const cv::Mat &gameplayBinary, const Logger &logger)
{
	cv::Rect rankRect = opponentRankExtractor.extract(gameplayBinary);
	cv::Mat rankImage = ~gameplayBinary(rankRect);
	logger.writeOpponentRankImage(logger.getPrefix(), rankImage);
	std::string recognizedText = recognizeText(rankImage);
	nlohmann::json json{
		{"text", recognizedText},
	};
	std::string jsonString(json.dump());
	sendEventToAllBrowserSources(EVENT_NAME_OPPONENT_RANK_SHOWN,
				     jsonString.c_str());
	return ScreenState::RANK_SHOWN;
}

static ScreenState handleRankShown(SceneDetector::Scene scene)
{
	if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else {
		return ScreenState::RANK_SHOWN;
	}
}

static ScreenState handleEnteringSelectPokemon(
	uint64_t lastStateChangedNs, const cv::Mat &gameplayBGRA,
	EntityCropper &opponentPokemonCropper,
	std::array<int, N_POKEMONS> &mySelectionOrderMap,
	const PokemonRecognizer &pokemonRecognizer, const Logger &logger)
{
	const uint64_t now = os_gettime_ns();
	if (now - lastStateChangedNs > 1000000000) {
		renderOpponentPokemons(gameplayBGRA, opponentPokemonCropper,
				       logger);
		mySelectionOrderMap.fill(0);
		std::vector<std::string> pokemonNames(N_POKEMONS);
		for (int i = 0; i < N_POKEMONS; i++) {
			const cv::Mat &imageBGRA =
				opponentPokemonCropper.imagesBGRA[i];
			pokemonNames[i] =
				pokemonRecognizer.recognizePokemon(imageBGRA);
		}
		logger.writeOpponentTeamText(logger.getPrefix(), pokemonNames);
		return ScreenState::SELECT_POKEMON;
	} else {
		return ScreenState::ENTERING_SELECT_POKEMON;
	}
}

static ScreenState
handleSelectPokemon(SceneDetector::Scene scene,
		    EntityCropper &selectionOrderCropper,
		    const cv::Mat &gameplayBGRA,
		    const SelectionRecognizer &selectionRecognizer,
		    std::array<int, N_POKEMONS> &mySelectionOrderMap,
		    EntityCropper &myPokemonCropper,
		    std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA)
{
	if (detectSelectionOrderChange(selectionOrderCropper, gameplayBGRA,
				       selectionRecognizer,
				       mySelectionOrderMap)) {
		drawMyPokemons(myPokemonCropper, gameplayBGRA, myPokemonsBGRA,
			       mySelectionOrderMap);
	}

	if (scene != SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::LEAVING_SELECT_POKEMON;
	} else {
		return ScreenState::SELECT_POKEMON;
	}
}

static ScreenState handleLeavingSelectPokemon(
	SceneDetector::Scene scene,
	const std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA,
	const Logger &logger)
{
	std::string prefix = logger.getPrefix();
	for (int i = 0; i < N_POKEMONS; i++) {
		logger.writeMyPokemonImage(prefix, i, myPokemonsBGRA[i]);
	}
	if (scene == SceneDetector::SCENE_UNDEFINED) {
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::UNKNOWN;
	}
}

static ScreenState handleEnteringConfirmPokemon(SceneDetector::Scene scene,
						uint64_t lastStateChangedNs)
{
	uint64_t now = os_gettime_ns();
	if (now - lastStateChangedNs > 500000000) {
		return ScreenState::CONFIRM_POKEMON;
	} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else {
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	}
}

static ScreenState handleConfirmPokemon(SceneDetector::Scene scene)
{
	if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::RANK_SHOWN;
	} else {
		return ScreenState::CONFIRM_POKEMON;
	}
}

static ScreenState handleEnteringMatch(SceneDetector::Scene scene,
				       SceneDetector::Scene prevScene)
{
	if (prevScene != SceneDetector::SCENE_BLACK_TRANSITION &&
	    scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		nlohmann::json json{{"durationMins", 20}};
		std::string jsonString(json.dump());
		sendEventToAllBrowserSources(
			"obsPokemonSvScreenBuilderMatchStarted",
			jsonString.c_str());
		return ScreenState::MATCH;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::ENTERING_MATCH;
	}
}

static ScreenState handleMatch(SceneDetector::Scene scene,
			       SceneDetector::Scene prevScene)
{
	if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		sendEventToAllBrowserSources(
			"obsPokemonSvScreenBuilderMatchEnded", "null");
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		sendEventToAllBrowserSources(
			"obsPokemonSvScreenBuilderMatchEnded", "null");
		return ScreenState::ENTERING_RANK_SHOWN;
	} else if (prevScene != SceneDetector::SCENE_BLACK_TRANSITION &&
		   scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		sendEventToAllBrowserSources(
			"obsPokemonSvScreenBuilderMatchEnded", "null");
		return ScreenState::ENTERING_RESULT;
	} else {
		return ScreenState::MATCH;
	}
}

static ScreenState handleEnteringResult()
{
	return ScreenState::RESULT;
}

static ScreenState handleResult(SceneDetector::Scene scene,
				uint64_t lastStateChangedNs)
{
	uint64_t now = os_gettime_ns();
	if (now - lastStateChangedNs > 2000000000) {
		return ScreenState::UNKNOWN;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else if (scene == SceneDetector::SCENE_RANK_SHOWN) {
		return ScreenState::ENTERING_RANK_SHOWN;
	} else {
		return ScreenState::RESULT;
	}
}
