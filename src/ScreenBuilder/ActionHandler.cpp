#include <nlohmann/json.hpp>

#include <obs.h>

#include "plugin-support.h"

#include "Base64/Base64.hpp"

#include "TextRecognizer/TextRecognizer.h"

#include "ActionHandler.hpp"
#include "MatchState.hpp"
#include "ObsBrowserUtil.hpp"

void ActionHandler::handleEnteringRankShown(const cv::Mat &gameplayGray,
					    MatchState &matchState) const
{
	std::string prefix = logger.getPrefix();

	matchState = {};
	matchState.timestamp = prefix;

	cv::Rect myRankRect = myRankExtractor(gameplayGray);
	if (myRankRect.empty()) {
		obs_log(LOG_INFO, "Failed to extract my rank!");
		logger.writeScreenshot(prefix, "MyRankFailed", gameplayGray);
	} else {
		cv::Mat rankGray = gameplayGray(myRankRect), rankBinary;
		cv::threshold(rankGray, rankBinary, 200, 255,
			      cv::THRESH_BINARY_INV);
		logger.writeMyRankImage(prefix, rankBinary);

		recognizeText(rankBinary, [&](std::string rankText) {
			obs_log(LOG_INFO, "My rank text is %s.",
				rankText.c_str());
			matchState.myRank = rankText;
			dispatchMyRankShown(rankText);
		});
	}

	cv::Mat opponentRankBinary;
	cv::threshold(gameplayGray, opponentRankBinary, 128, 255,
		      cv::THRESH_BINARY);
	cv::Rect opponentRankRect = opponentRankExtractor(opponentRankBinary);
	if (opponentRankRect.empty()) {
		obs_log(LOG_INFO, "Failed to extract the opponent rank!");
		logger.writeScreenshot(prefix, "OpponentRankFailed",
				       gameplayGray);
	} else {
		cv::Mat rankGray = gameplayGray(opponentRankRect), rankBinary;
		cv::threshold(rankGray, rankBinary, 200, 255,
			      cv::THRESH_BINARY_INV);
		logger.writeOpponentRankImage(prefix, rankBinary);

		recognizeText(rankBinary, [&](std::string rankText) {
			obs_log(LOG_INFO, "The opponent rank text is %s.",
				rankText.c_str());
			matchState.opponentRank = rankText;
			dispatchOpponentRankShown(rankText);
		});
	}
}

void ActionHandler::handleEnteringSelectPokemon(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
	bool canEnterToSelectPokemon, std::vector<int> &mySelectionOrderMap,
	MatchState &matchState) const
{
	if (canEnterToSelectPokemon) {
		for (size_t i = 0; i < mySelectionOrderMap.size(); i++) {
			mySelectionOrderMap[i] = 0;
		}

		std::vector<cv::Mat> imagesBGRA =
			opponentPokemonCropper.crop(gameplayBGRA);
		std::vector<cv::Mat> imagesBGR =
			opponentPokemonCropper.crop(gameplayBGR);
		std::vector<cv::Mat> masks =
			opponentPokemonCropper.generateMask(imagesBGR);
		std::vector<cv::Mat> resultsBGRA =
			opponentPokemonCropper.generateTransparentImages(
				imagesBGRA, masks);
		std::vector<std::string> pokemonNames(resultsBGRA.size());
		for (size_t i = 0; i < pokemonNames.size(); i++) {
			pokemonNames[i] =
				opponentPokemonImageRecognizer(resultsBGRA[i]);
		}
		matchState.opponentPokemonIds = pokemonNames;
		dispatchOpponentTeamShown(resultsBGRA, pokemonNames);
	}
}

bool ActionHandler::detectSelectionOrderChange(
	const cv::Mat &gameplayBGR, const cv::Mat &gameplayGray,
	std::vector<int> &mySelectionOrderMap) const
{
	std::vector<cv::Mat> imagesBGR =
		selectionOrderCropper.crop(gameplayBGR);
	std::vector<cv::Mat> imagesGray =
		selectionOrderCropper.crop(gameplayGray);
	std::vector<int> orders(imagesBGR.size());
	bool change_detected = false;
	for (size_t i = 0; i < orders.size(); i++) {
		orders[i] = mySelectionRecognizer(imagesBGR[i], imagesGray[i]);
		int currentPokemon = static_cast<int>(i + 1);
		if (orders[i] > 0 &&
		    mySelectionOrderMap[orders[i] - 1] != currentPokemon) {
			mySelectionOrderMap[orders[i] - 1] = currentPokemon;
			change_detected = true;
		}
	}
	if (change_detected) {
		blog(LOG_INFO, "My order: %d %d %d %d %d %d", orders[0],
		     orders[1], orders[2], orders[3], orders[4], orders[5]);
	}
	return change_detected;
}

void ActionHandler::recognizeMyPokemons(
	MatchState &matchState, const std::vector<cv::Mat> &myPokemonImagesBGRA,
	const std::vector<cv::Mat> &myPokemonImagesGray, size_t i) const
{
	if (i == myPokemonImagesGray.size()) {
		dispatchMySelectionChanged(myPokemonImagesBGRA,
					   matchState.mySelectionMap,
					   matchState.myPokemonNames,
					   matchState.myToolNames);
		return;
	}

	if (myPokemonImagesGray.at(i).empty()) {
		recognizeMyPokemons(matchState, myPokemonImagesBGRA,
				    myPokemonImagesGray, i + 1);
		return;
	}

	myPokemonNameRecognizer(
		myPokemonImagesGray.at(i), [&, i](std::string myPokemonName) {
			matchState.myPokemonNames.at(i) = myPokemonName;
			myToolNameRecognizer(
				myPokemonImagesGray.at(i),
				[&, i](std::string myToolName) {
					matchState.myToolNames.at(i) =
						(myToolName);
					recognizeMyPokemons(matchState,
							    myPokemonImagesBGRA,
							    myPokemonImagesGray,
							    i + 1);
				});
		});
}

void ActionHandler::handleSelectPokemon(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
	const cv::Mat &gameplayHSV, const cv::Mat &gameplayGray,
	std::vector<int> &mySelectionMap,
	std::vector<cv::Mat> &myPokemonImagesBGRA,
	std::vector<cv::Mat> &myPokemonImagesGray, MatchState &matchState) const
{
	if (!detectSelectionOrderChange(gameplayBGR, gameplayGray,
					mySelectionMap)) {
		return;
	}

	const auto croppedBGRA = myPokemonCropper.crop(gameplayBGRA);
	const auto croppedGray = myPokemonCropper.crop(gameplayGray);
	const auto shouldUpdate = myPokemonCropper.getShouldUpdate(gameplayHSV);

	for (size_t i = 0; i < myPokemonImagesBGRA.size(); i++) {
		if (shouldUpdate.at(i)) {
			myPokemonImagesBGRA.at(i) = croppedBGRA.at(i).clone();
			myPokemonImagesGray.at(i) = croppedGray.at(i).clone();
		}
	}

	matchState.mySelectionMap = mySelectionMap;
	recognizeMyPokemons(matchState, myPokemonImagesBGRA,
			    myPokemonImagesGray);
}

void ActionHandler::handleEnteringMatch(bool canEnterToMatch) const
{
	if (canEnterToMatch) {
		dispatchMatchStarted(20);
	}
}

void ActionHandler::handleResult(const cv::Mat &gameplayHSV,
				 MatchState &matchState) const
{
	const auto prefix = logger.getPrefix();
	const auto images = resultCropper.crop(gameplayHSV);
	const auto resultString = resultRecognizer(images[0]);

	matchState.resultString = resultString;

	logger.writeScreenshot(prefix, "MatchResultShown", gameplayHSV);
	logger.writeResultImage(prefix, images[0]);
	logger.writeMatchState(prefix, matchState);

	dispatchMatchCompleted(prefix, resultString);
}
