#pragma once

#include <array>
#include <vector>

#include <obs.h>

#include "Croppers/MyPokemonCropper.hpp"
#include "factory.hpp"
#include "modules/EntityCropper.h"
#include "modules/Logger.hpp"
#include "modules/MyRankExtractor.h"
#include "modules/OpponentRankExtractor.h"
#include "Recognizers/PokemonRecognizer.h"
#include "Recognizers/SelectionRecognizer.h"
#include "ScreenBuilder/Automaton.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/SceneDetector.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

const std::array<int, 2> opponent_col_range{1239, 1337};
const std::vector<std::array<int, 2>> opponent_row_range{{{228, 326},
							  {330, 428},
							  {432, 530},
							  {534, 632},
							  {636, 734},
							  {738, 836}}};

const std::array<int, 2> myPokemonColRange{182, 711};
const std::vector<std::array<int, 2>> myPokemonRowRange{{{147, 254},
							 {263, 371},
							 {379, 486},
							 {496, 602},
							 {612, 718},
							 {727, 834}}};

const std::array<int, 2> selectionOrderColRange{795, 827};
const std::vector<std::array<int, 2>> selectionOrderRowRange{{{154, 186},
							      {271, 303},
							      {388, 420},
							      {503, 535},
							      {619, 651},
							      {735, 767}}};

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

	EntityCropper opponentPokemonCropper;
	MyPokemonCropper myPokemonCropper;
	EntityCropper selectionOrderCropper;
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
		: opponentPokemonCropper(opponent_col_range,
					 opponent_row_range),
		  selectionOrderCropper(selectionOrderColRange,
					selectionOrderRowRange),
		  actionHandler(myRankExtractor, opponentRankExtractor, logger,
				pokemonRecognizer, opponentPokemonCropper,
				selectionOrderCropper, selectionRecognizer,
				myPokemonCropper),
		  pokemonRecognizer(factory::newPokemonRecognizer(
			  "preset/PokemonRecognizer.cbor")),
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
