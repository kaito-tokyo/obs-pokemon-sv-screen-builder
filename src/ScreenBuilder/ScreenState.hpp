#pragma once

#include <string>

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

std::string screenStateToString(ScreenState state)
{
	switch (state) {
	case ScreenState::UNKNOWN:
		return "UNKNOWN";
	case ScreenState::ENTERING_RANK_SHOWN:
		return "ENTERING_RANK_SHOWN";
	case ScreenState::RANK_SHOWN:
		return "RANK_SHOWN";
	case ScreenState::ENTERING_SELECT_POKEMON:
		return "ENTERING_SELECT_POKEMON";
	case ScreenState::SELECT_POKEMON:
		return "SELECT_POKEMON";
	case ScreenState::LEAVING_SELECT_POKEMON:
		return "LEAVING_SELECT_POKEMON";
	case ScreenState::ENTERING_CONFIRM_POKEMON:
		return "ENTERING_CONFIRM_POKEMON";
	case ScreenState::CONFIRM_POKEMON:
		return "CONFIRM_POKEMON";
	case ScreenState::ENTERING_MATCH:
		return "ENTERING_MATCH";
	case ScreenState::MATCH:
		return "MATCH";
	case ScreenState::ENTERING_RESULT:
		return "ENTERING_RESULT";
	case ScreenState::RESULT:
		return "RESULT";
	default:
		return "UNKNOWN";
	}
}
