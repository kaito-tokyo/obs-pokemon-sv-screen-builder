#pragma once

#include <map>

#include <nlohmann/json.hpp>

#include "modules/SceneDetector.h"
#include "modules/OpponentRankExtractor.h"
#include "modules/TextRecognizer.h"
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
handleEnteringShowRank(OpponentRankExtractor &opponentRankExtractor,
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
    }
}
