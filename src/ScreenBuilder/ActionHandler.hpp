#pragma once

#include "modules/Logger.hpp"
#include "modules/OpponentRankExtractor.h"
#include "modules/MyRankExtractor.h"
#include "modules/PokemonRecognizer.h"
#include "modules/EntityCropper.h"
#include "constants.h"
#include "modules/SelectionRecognizer.h"
#include "Croppers/MyPokemonCropper.hpp"

class ActionHandler {
public:
	const MyRankExtractor &myRankExtractor;
	const OpponentRankExtractor &opponentRankExtractor;
	const PokemonRecognizer &pokemonRecognizer;
	const Logger &logger;
	EntityCropper &opponentPokemonCropper;
	EntityCropper &selectionOrderCropper;
	const SelectionRecognizer &selectionRecognizer;
	const MyPokemonCropper &myPokemonCropper;


	ActionHandler(const MyRankExtractor &_myRankExtractor,
		      const OpponentRankExtractor &_opponentRankExtractor,
			  const Logger &_logger,
			  const PokemonRecognizer &_pokemonRecognizer,
			  EntityCropper &_opponentPokemonCropper,
			  EntityCropper &_selectionOrderCropper,
			  const SelectionRecognizer &_selectionRecognizer,
			  const MyPokemonCropper &_myPokemonCropper)
		: myRankExtractor(_myRankExtractor),
		  opponentRankExtractor(_opponentRankExtractor),
		  logger(_logger),
		  pokemonRecognizer(_pokemonRecognizer),
		  opponentPokemonCropper(_opponentPokemonCropper),
		  selectionOrderCropper(_selectionOrderCropper),
		  selectionRecognizer(_selectionRecognizer),
		  myPokemonCropper(_myPokemonCropper)
	{
	}

	void handleEnteringRankShown(const cv::Mat &gameplayGray) const;
	void handleEnteringSelectPokemon(
	const cv::Mat &gameplayBGRA,
	bool canEnterToSelectPokemon,
	std::array<int, N_POKEMONS> &mySelectionOrderMap) const;
	void handleSelectPokemon(const cv::Mat &gameplayBGRA, const cv::Mat &gameplayHsv,
		    std::array<int, N_POKEMONS> &mySelectionOrderMap,
		    std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA) const;
	void handleEnteringMatch(bool canEnterToMatch) const;
};
