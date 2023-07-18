#pragma once

#include <map>

#include "modules/SceneDetector.h"
#include "modules/OpponentRankExtractor.h"

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
