#pragma once

#include <array>
#include <vector>

#include <obs.h>

#include "factory.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/Logger.hpp"
#include "ScreenBuilder/MatchStateAggregator.hpp"
#include "ScreenBuilder/SceneDetector.hpp"
#include "ScreenBuilder/StateMachine.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

struct screen_context {
	obs_source_t *source = nullptr;
	obs_source_t *gameplaySource = nullptr;
	std::string customData;

	gs_texrender_t *texrender = nullptr;
	gs_stagesurf_t *stagesurface = nullptr;
	cv::Mat gameplayBGRA;

	std::string latestVersion;

	MyPokemonCropper myPokemonCropper;
	OpponentPokemonCropper opponentPokemonCropper;
	ResultCropper resultCropper;
	SelectionOrderCropper selectionOrderCropper;
	MyRankExtractor myRankExtractor;
	OpponentRankExtractor opponentRankExtractor;
	MyPokemonNameRecognizer myPokemonNameRecognizer;
	MySelectionRecognizer mySelectionRecognizer;
	MyToolNameRecognizer myToolNameRecognizer;
	OpponentPokemonImageRecognizer opponentPokemonImageRecognizer;
	ResultRecognizer resultRecognizer;
	Logger logger;
	ActionHandler actionHandler;

	TemplateClassifier lobbyRankShown;
	HistClassifier lobbyMySelect;
	HistClassifier lobbyMySelectV;
	HistClassifier lobbyOpponentSelect;
	HistClassifier blackTransition;
	SceneDetector sceneDetector;

	StateMachine stateMachine;

	MatchStateAggregator matchStateAggregator;

	int nthTick = 0;

	screen_context()
		: myPokemonCropper(factory::newMyPokemonCropper(
			  "preset/MyPokemonCropper.json")),
		  opponentPokemonCropper(factory::newOpponentPokemonCropper(
			  "preset/OpponentPokemonCropper.json")),
		  resultCropper(factory::newResultCropper(
			  "preset/ResultCropper.json")),
		  selectionOrderCropper(factory::newSelectionOrderCropper(
			  "preset/SelectionOrderCropper.json")),
		  myRankExtractor(factory::newMyRankExtractor(
			  "preset/MyRankExtractor.cbor")),
		  opponentRankExtractor(factory::newOpponentRankExtractor(
			  "preset/OpponentRankExtractor.cbor")),
		  myPokemonNameRecognizer(factory::newMyPokemonNameRecognizer(
			  "preset/MyPokemonNameRecognizer.json")),
		  mySelectionRecognizer(factory::newMySelectionRecognizer(
			  "preset/MySelectionRecognizer.cbor")),
		  myToolNameRecognizer(factory::newMyToolNameRecognizer(
			  "preset/MyToolNameRecognizer.json")),
		  opponentPokemonImageRecognizer(
			  factory::newOpponentPokemonImageRecognizer(
				  "preset/OpponentPokemonImageRecognizer.cbor")),
		  resultRecognizer(factory::newResultRecognizer(
			  "preset/ResultRecognizer.json")),
		  actionHandler(myPokemonCropper, opponentPokemonCropper,
				resultCropper, selectionOrderCropper,
				myRankExtractor, opponentRankExtractor,
				myPokemonNameRecognizer, mySelectionRecognizer,
				myToolNameRecognizer,
				opponentPokemonImageRecognizer,
				resultRecognizer, logger, customData),
		  lobbyRankShown(factory::newTemplateClassifier(
			  "preset/SceneDetector_lobbyRankShown.cbor")),
		  lobbyMySelect(factory::newHistClassifier(
			  "preset/SceneDetector_lobbyMySelect.json")),
		  lobbyMySelectV(factory::newHistClassifier(
			  "preset/SceneDetector_lobbyMySelectV.json")),
		  lobbyOpponentSelect(factory::newHistClassifier(
			  "preset/SceneDetector_lobbyOpponentSelect.json")),
		  blackTransition(factory::newHistClassifier(
			  "preset/SceneDetector_blackTransition.json")),
		  sceneDetector(lobbyRankShown, lobbyMySelect, lobbyMySelectV,
				lobbyOpponentSelect, blackTransition),
		  stateMachine(actionHandler, sceneDetector),
		  matchStateAggregator(factory::newMatchStateAggregator(
			  "preset/MatchStateAggregator.json"))
	{
	}
};
