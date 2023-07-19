#pragma once

#include <map>

#include <nlohmann/json.hpp>

#include "modules/SceneDetector.h"
#include "modules/OpponentRankExtractor.h"
#include "modules/TextRecognizer.h"

#include "constants.h"
#include "renderers.h"
#include "obs-browser-api.h"

enum class ScreenState {
	UNKNOWN,
	ENTERING_SHOW_RANK,
	SHOW_RANK,
	ENTERING_SELECT_POKEMON,
	SELECT_POKEMON,
	ENTERING_CONFIRM_POKEMON,
	CONFIRM_POKEMON,
	ENTERING_MATCH,
	MATCH,
	ENTERING_RESULT,
	RESULT,
};

const std::map<ScreenState, const char *> ScreenStateNames = {
	{ScreenState::UNKNOWN, "UNKNOWN"},
	{ScreenState::ENTERING_SHOW_RANK, "ENTERING_SHOW_RANK"},
	{ScreenState::SHOW_RANK, "SHOW_RANK"},
	{ScreenState::ENTERING_SELECT_POKEMON, "ENTERING_SELECT_POKEMON"},
	{ScreenState::SELECT_POKEMON, "SELECT_POKEMON"},
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
	if (scene == SceneDetector::SCENE_SHOW_RANK) {
		return ScreenState::ENTERING_SHOW_RANK;
	} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
		return ScreenState::ENTERING_SELECT_POKEMON;
	} else {
		return ScreenState::UNKNOWN;
	}
}

static ScreenState
handleEnteringShowRank(const OpponentRankExtractor &opponentRankExtractor,
		       const cv::Mat &gameplayBinary)
{
	cv::Rect rankRect = opponentRankExtractor.extract(gameplayBinary);
	std::string recognizedText = recognizeText(~gameplayBinary(rankRect));
	nlohmann::json json{
		{"text", recognizedText},
	};
	std::string jsonString(json.dump());
	sendEventToAllBrowserSources(EVENT_NAME_OPPONENT_RANK_SHOWN,
				     jsonString.c_str());
	return ScreenState::SHOW_RANK;
}

static ScreenState handleShowRank(SceneDetector::Scene scene)
{
    if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
        return ScreenState::ENTERING_SELECT_POKEMON;
    } else {
        return ScreenState::SHOW_RANK;
    }
}

static ScreenState handleEnteringSelectPokemon(uint64_t lastStateChangedNs, const cv::Mat &gameplayBGRA, EntityCropper &opponentPokemonCropper, std::array<int, N_POKEMONS> &mySelectionOrderMap)
{
    const uint64_t now = os_gettime_ns();
    if (now - lastStateChangedNs > 1000000000) {
        renderOpponentPokemons(gameplayBGRA, opponentPokemonCropper);
        mySelectionOrderMap.fill(0);
        return ScreenState::SELECT_POKEMON;
    } else {
        return ScreenState::ENTERING_SELECT_POKEMON;
    }
}

static ScreenState handleSelectPokemon(SceneDetector::Scene scene, EntityCropper &selectionOrderCropper, const cv::Mat &gameplayBGRA, const SelectionRecognizer &selectionRecognizer, std::array<int, N_POKEMONS> &mySelectionOrderMap, EntityCropper &myPokemonCropper, std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA)
{
    if (detectSelectionOrderChange(selectionOrderCropper, gameplayBGRA, selectionRecognizer, mySelectionOrderMap)) {
        drawMyPokemons(myPokemonCropper, gameplayBGRA, myPokemonsBGRA, mySelectionOrderMap);
    }

    if (scene == SceneDetector::SCENE_UNDEFINED) {
		return ScreenState::ENTERING_CONFIRM_POKEMON;
    } else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else if (scene == SceneDetector::SCENE_SHOW_RANK) {
		return ScreenState::ENTERING_SHOW_RANK;
    } else {
		return ScreenState::SELECT_POKEMON;
	}
}

static ScreenState handleEnteringConfirmPokemon(SceneDetector::Scene scene, uint64_t lastStateChangedNs) {
	uint64_t now = os_gettime_ns();
	if (now - lastStateChangedNs > 500000000) {
		return ScreenState::CONFIRM_POKEMON;
	} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
		return ScreenState::ENTERING_MATCH;
	} else {
		return ScreenState::ENTERING_CONFIRM_POKEMON;
	}
}
