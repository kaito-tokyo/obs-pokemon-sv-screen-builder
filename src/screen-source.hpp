#pragma once

#include <array>
#include <vector>

#include <obs.h>

#include "factory.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/Logger.hpp"
#include "ScreenBuilder/SceneDetector.hpp"
#include "ScreenBuilder/StateMachine.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

struct screen_context {
	obs_source_t *source = nullptr;
	obs_source_t *gameplaySource = nullptr;

	gs_texrender_t *texrender = nullptr;
	gs_stagesurf_t *stagesurface = nullptr;
	cv::Mat gameplayBGRA;

	MyPokemonCropper myPokemonCropper;
	OpponentPokemonCropper opponentPokemonCropper;
	ResultCropper resultCropper;
	SelectionOrderCropper selectionOrderCropper;
	MyRankExtractor myRankExtractor;
	OpponentRankExtractor opponentRankExtractor;
	PokemonRecognizer pokemonRecognizer;
	ResultRecognizer resultRecognizer;
	SelectionRecognizer selectionRecognizer;
	Logger logger;
	ActionHandler actionHandler;

	TemplateClassifier lobbyRankShown;
	HistClassifier lobbyMySelect;
	HistClassifier lobbyOpponentSelect;
	HistClassifier blackTransition;
	SceneDetector sceneDetector;

	StateMachine stateMachine;

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
		  pokemonRecognizer(factory::newPokemonRecognizer(
			  "preset/PokemonRecognizer.cbor")),
		  resultRecognizer(factory::newResultRecognizer(
			  "preset/ResultRecognizer.json")),
		  selectionRecognizer(factory::newSelectionRecognizer(
			  "preset/SelectionRecognizer.cbor")),
		  actionHandler(myPokemonCropper, opponentPokemonCropper,
				resultCropper, selectionOrderCropper,
				myRankExtractor, opponentRankExtractor,
				pokemonRecognizer, resultRecognizer,
				selectionRecognizer, logger),
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
		  stateMachine(actionHandler, sceneDetector)
	{
	}
};
