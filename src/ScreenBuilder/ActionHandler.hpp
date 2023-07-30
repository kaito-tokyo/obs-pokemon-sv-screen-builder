#pragma once

#include "Croppers/MyPokemonCropper.hpp"
#include "Croppers/OpponentPokemonCropper.hpp"
#include "Croppers/SelectionOrderCropper.hpp"
#include "Extractors/MyRankExtractor.hpp"
#include "Extractors/OpponentRankExtractor.hpp"
#include "Recognizers/PokemonRecognizer.hpp"
#include "Recognizers/SelectionRecognizer.hpp"
#include "ScreenBuilder/Logger.hpp"

class ActionHandler {
public:
	const MyRankExtractor &myRankExtractor;
	const OpponentRankExtractor &opponentRankExtractor;
	const PokemonRecognizer &pokemonRecognizer;
	const Logger &logger;
	const SelectionOrderCropper &selectionOrderCropper;
	const SelectionRecognizer &selectionRecognizer;
	const MyPokemonCropper &myPokemonCropper;
	const OpponentPokemonCropper &opponentPokemonCropper;

	ActionHandler(const MyRankExtractor &_myRankExtractor,
		      const OpponentRankExtractor &_opponentRankExtractor,
		      const Logger &_logger,
		      const PokemonRecognizer &_pokemonRecognizer,
		      const SelectionOrderCropper &_selectionOrderCropper,
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
		std::vector<int> &mySelectionOrderMap) const;
	void handleSelectPokemon(
		const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
		const cv::Mat &gameplayHsv, const cv::Mat &gameplayGray,
		std::vector<int> &mySelectionOrderMap,
		std::vector<cv::Mat> &myPokemonsBGRA) const;
	void handleEnteringMatch(bool canEnterToMatch) const;

private:
	bool detectSelectionOrderChange(
		const cv::Mat &gameplayBGR, const cv::Mat &gameplayGray,
		std::vector<int> &mySelectionOrderMap) const;
	void drawMyPokemons(
		const cv::Mat &gameplayBGRA, const cv::Mat &gameplayHSV,
		std::vector<cv::Mat> &myPokemonsBGRA,
		const std::vector<int> &mySelectionOrderMap) const;
};
