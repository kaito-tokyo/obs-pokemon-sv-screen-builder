#pragma once

#include <array>
#include <vector>

#include <obs.h>

#include "modules/EntityCropper.h"
#include "modules/OpponentRankExtractor.h"
#include "modules/SceneDetector.h"
#include "modules/SelectionRecognizer.h"

enum screen_state {
	STATE_UNKNOWN,
	STATE_ENTERING_SELECT_POKEMON,
	STATE_SELECT_POKEMON,
	STATE_ENTERING_CONFIRM_POKEMON,
	STATE_CONFIRM_POKEMON,
	STATE_ENTERING_MATCH,
	STATE_MATCH,
	STATE_ENTERING_RESULT,
	STATE_RESULT,
};

const HistClassifier classifier_lobby_my_select = {.rangeCol = {149, 811},
						   .rangeRow = {139, 842},
						   .histChannel = 0,
						   .histBins = 30,
						   .histMaxIndex = 17,
						   .histRatio = 0.5};
const HistClassifier classifier_lobby_opponent_select = {.rangeCol = {1229,
								      1649},
							 .rangeRow = {227, 836},
							 .histChannel = 0,
							 .histBins = 30,
							 .histMaxIndex = 0,
							 .histRatio = 0.8};
const HistClassifier classifier_black_transition = {.rangeCol = {400, 600},
						    .rangeRow = {400, 600},
						    .histChannel = 2,
						    .histBins = 8,
						    .histMaxIndex = 0,
						    .histRatio = 0.8};

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

const int N_POKEMONS = 6;

struct screen_context {
	obs_data_t *settings = nullptr;
	obs_source_t *source = nullptr;

	gs_texrender_t *texrender = nullptr;
	gs_stagesurf_t *stagesurface = nullptr;
	cv::Mat gameplay_bgra;

	uint64_t next_tick = 0;
	SceneDetector sceneDetector;

	screen_state state = STATE_UNKNOWN;
	uint64_t last_state_change_ns = 0;
	int my_selection_order_map[N_POKEMONS]{};
	SceneDetector::Scene prev_scene;
	uint64_t match_start_ns = 0;
	uint64_t last_elapsed_seconds = 0;
	uint64_t match_end_ns = 0;

	EntityCropper opponentPokemonCropper;
	EntityCropper myPokemonCropper;
	EntityCropper selectionOrderCropper;
	SelectionRecognizer selectionRecognizer;
	OpponentRankExtractor opponentRankExtractor;

	cv::Mat myPokemonsBGRA[N_POKEMONS];

	screen_context()
		: sceneDetector(classifier_lobby_my_select,
				classifier_lobby_opponent_select,
				classifier_black_transition),
		  opponentPokemonCropper(opponent_col_range,
					 opponent_row_range),
		  myPokemonCropper(myPokemonColRange, myPokemonRowRange),
		  selectionOrderCropper(selectionOrderColRange,
					selectionOrderRowRange)
	{
	}
};
