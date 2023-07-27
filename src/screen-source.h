#pragma once

#include <array>
#include <vector>

#include <obs.h>

#include "Croppers/MyPokemonCropper.hpp"
#include "factory.hpp"
#include "modules/Logger.hpp"
#include "modules/MyRankExtractor.h"
#include "modules/OpponentRankExtractor.h"
#include "Recognizers/PokemonRecognizer.h"
#include "Recognizers/SelectionRecognizer.hpp"
#include "ScreenBuilder/Automaton.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/SceneDetector.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

struct screen_context {
	obs_source_t *source = nullptr;
	obs_source_t *gameplaySource = nullptr;

	gs_texrender_t *texrender = nullptr;
	gs_stagesurf_t *stagesurface = nullptr;
	cv::Mat gameplay_bgra;
	cv::Mat gameplayHsv;
	cv::Mat gameplayGray;
	cv::Mat gameplayBinary;

	uint64_t next_tick = 0;

	ScreenState state = ScreenState::UNKNOWN;
	uint64_t last_state_change_ns = 0;
	std::array<int, N_POKEMONS> my_selection_order_map;

	OpponentPokemonCropper opponentPokemonCropper;
	MyPokemonCropper myPokemonCropper;
	SelectionOrderCropper selectionOrderCropper;
	SelectionRecognizer selectionRecognizer;
	OpponentRankExtractor opponentRankExtractor;
	Logger logger;
	PokemonRecognizer pokemonRecognizer;
	MyRankExtractor myRankExtractor;

	TemplateClassifier lobbyRankShown;
	HistClassifier lobbyMySelect;
	HistClassifier lobbyOpponentSelect;
	HistClassifier blackTransition;
	SceneDetector sceneDetector;
	ActionHandler actionHandler;
	Automaton automaton;

	std::array<cv::Mat, N_POKEMONS> myPokemonsBGRA;

	screen_context()
		: opponentPokemonCropper(factory::newOpponentPokemonCropper(
			  "preset/OpponentPokemonCropper.json")),
		  selectionOrderCropper(factory::newSelectionOrderCropper(
			  "preset/SelectionOrderCropper.json")),
		  actionHandler(myRankExtractor, opponentRankExtractor, logger,
				pokemonRecognizer, selectionOrderCropper,
				selectionRecognizer, myPokemonCropper,
				opponentPokemonCropper),
		  pokemonRecognizer(factory::newPokemonRecognizer(
			  "preset/PokemonRecognizer.cbor")),
		  selectionRecognizer(factory::newSelectionRecognizer(
			  "preset/SelectionRecognizer.cbor")),
		  myPokemonCropper(factory::newMyPokemonCropper(
			  "preset/MyPokemonCropper.json")),
		  lobbyRankShown(factory::newTemplateClassifier(
			  "preset/SceneDetector_lobbyRankShown.cbor")),
		  lobbyMySelect(factory::newHistClassifier(
			  "preset/SceneDetector_lobbyMySelect.json")),
		  lobbyOpponentSelect(factory::newHistClassifier(
			  "preset/SceneDetector_lobbyOpponentSelect.json")),
		  blackTransition(factory::newHistClassifier(
			  "preset/SceneDetector_blackTransition.json")),
		  sceneDetector(lobbyRankShown, lobbyMySelect,
				lobbyOpponentSelect, blackTransition),
		  automaton(actionHandler, sceneDetector)
	{
	}
};
