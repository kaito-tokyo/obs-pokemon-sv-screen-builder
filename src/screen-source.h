#pragma once

#include <array>
#include <vector>

#include <obs.h>

#include "modules/EntityCropper.h"
#include "modules/OpponentRankExtractor.h"
#include "modules/SceneDetector.h"
#include "modules/SelectionRecognizer.h"
#include "modules/Logger.hpp"
#include "state-machine.h"

const HistClassifier classifier_lobby_my_select = {{149, 811}, {139, 842}, 0,
						   30,         17,         0.5};
const HistClassifier classifier_lobby_opponent_select = {
	{1229, 1649}, {227, 836}, 0, 30, 0, 0.8};
const HistClassifier classifier_black_transition = {
	{400, 600}, {400, 600}, 2, 8, 0, 0.8};

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
	SceneDetector sceneDetector;

	ScreenState state = ScreenState::UNKNOWN;
	uint64_t last_state_change_ns = 0;
	std::array<int, N_POKEMONS> my_selection_order_map;
	SceneDetector::Scene prev_scene;

	EntityCropper opponentPokemonCropper;
	EntityCropper myPokemonCropper;
	EntityCropper selectionOrderCropper;
	SelectionRecognizer selectionRecognizer;
	OpponentRankExtractor opponentRankExtractor;
	Logger logger;

	std::array<cv::Mat, N_POKEMONS> myPokemonsBGRA;

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
