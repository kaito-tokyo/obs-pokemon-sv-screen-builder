#pragma once

#include "modules/Logger.hpp"
#include "modules/OpponentRankExtractor.h"
#include "modules/MyRankExtractor.h"
#include "Recognizers/PokemonRecognizer.h"
#include "modules/EntityCropper.h"
#include "constants.h"
#include "Recognizers/SelectionRecognizer.hpp"
#include "Croppers/MyPokemonCropper.hpp"
#include "Croppers/OpponentPokemonCropper.hpp"

class ActionHandler {
public:
	const MyRankExtractor &myRankExtractor;
	const OpponentRankExtractor &opponentRankExtractor;
	const PokemonRecognizer &pokemonRecognizer;
	const Logger &logger;
	EntityCropper &selectionOrderCropper;
	const SelectionRecognizer &selectionRecognizer;
	const MyPokemonCropper &myPokemonCropper;
	const OpponentPokemonCropper &opponentPokemonCropper;

	ActionHandler(const MyRankExtractor &_myRankExtractor,
		      const OpponentRankExtractor &_opponentRankExtractor,
		      const Logger &_logger,
		      const PokemonRecognizer &_pokemonRecognizer,
		      EntityCropper &_selectionOrderCropper,
		      const SelectionRecognizer &_selectionRecognizer,
		      const MyPokemonCropper &_myPokemonCropper,
		      const OpponentPokemonCropper &_opponentPokemonCropper)
		: myRankExtractor(_myRankExtractor),
		  opponentRankExtractor(_opponentRankExtractor),
		  logger(_logger),
		  pokemonRecognizer(_pokemonRecognizer),
		  selectionOrderCropper(_selectionOrderCropper),
		  selectionRecognizer(_selectionRecognizer),
		  myPokemonCropper(_myPokemonCropper),
		  opponentPokemonCropper(_opponentPokemonCropper)
	{
	}

	void handleEnteringRankShown(const cv::Mat &gameplayGray) const;
	void handleEnteringSelectPokemon(
		const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
		bool canEnterToSelectPokemon,
		std::array<int, N_POKEMONS> &mySelectionOrderMap) const;
	void handleSelectPokemon(
		const cv::Mat &gameplayBGRA, const cv::Mat &gameplayHsv,
		std::array<int, N_POKEMONS> &mySelectionOrderMap,
		std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA) const;
	void handleEnteringMatch(bool canEnterToMatch) const;
};
