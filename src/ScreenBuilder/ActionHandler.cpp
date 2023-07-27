#include <nlohmann/json.hpp>

#include <obs.h>

#include "plugin-support.h"

#include "ActionHandler.hpp"
#include "TextRecognizer/TextRecognizer.h"
#include "modules/EntityCropper.h"
#include "obs-browser-api.h"
#include "constants.h"
#include "modules/Base64.hpp"

static void dispatchMyRankShown(std::string text)
{
	nlohmann::json json{{"text", text}};
	std::string jsonString(json.dump());
	sendEventToAllBrowserSources("obsPokemonSvScreenBuilderMyRankShown",
				     jsonString.c_str());
}

static void dispatchOpponentRankShown(std::string text)
{
	nlohmann::json json{{"text", text}};
	std::string jsonString(json.dump());
	sendEventToAllBrowserSources(
		"obsPokemonSvScreenBuilderOpponentRankShown",
		jsonString.c_str());
}

static void dispatchOpponentTeamShown(const std::vector<cv::Mat> &images)
{
	std::vector<std::string> imageUrls;
	for (size_t i = 0; i < images.size(); i++) {
		std::vector<uchar> pngImage;
		cv::imencode(".png", images[i], pngImage);
		imageUrls.push_back("data:image/png;base64," +
				    Base64::encode(pngImage));
	}

	nlohmann::json json{{"imageUrls", imageUrls}};
	std::string jsonString = json.dump();
	sendEventToAllBrowserSources(
		"obsPokemonSvScreenBuilderOpponentTeamShown",
		jsonString.c_str());
}

void ActionHandler::handleEnteringRankShown(const cv::Mat &gameplayGray) const
{
	std::string prefix = logger.getPrefix();

	cv::Mat gameplayBinary;
	cv::threshold(gameplayGray, gameplayBinary, 200, 255,
		      cv::THRESH_BINARY);

	cv::Rect myRankRect = myRankExtractor.extract(gameplayBinary);
	if (myRankRect.empty()) {
		obs_log(LOG_INFO, "Failed to extract my rank!");
		logger.writeScreenshot(prefix, "MyRankFailed", gameplayBinary);
	} else {
		cv::Mat rankImage = ~gameplayBinary(myRankRect);
		logger.writeMyRankImage(prefix, rankImage);

		std::string rankText = recognizeText(rankImage);
		obs_log(LOG_INFO, "My rank text is %s.", rankText.c_str());

		dispatchMyRankShown(rankText);
	}

	cv::Mat opponentRankBinary;
	cv::threshold(gameplayGray, opponentRankBinary, 128, 255,
		      cv::THRESH_BINARY);
	cv::Rect opponentRankRect =
		opponentRankExtractor.extract(opponentRankBinary);
	if (opponentRankRect.empty()) {
		obs_log(LOG_INFO, "Failed to extract the opponent rank!");
		logger.writeScreenshot(prefix, "OpponentRankFailed",
				       gameplayBinary);
	} else {
		cv::Mat rankImage = ~gameplayBinary(opponentRankRect);
		logger.writeOpponentRankImage(prefix, rankImage);

		std::string rankText = recognizeText(rankImage);
		obs_log(LOG_INFO, "The opponent rank text is %s.",
			rankText.c_str());

		dispatchOpponentRankShown(rankText);
	}
}

void ActionHandler::handleEnteringSelectPokemon(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
	bool canEnterToSelectPokemon,
	std::array<int, N_POKEMONS> &mySelectionOrderMap) const
{
	if (canEnterToSelectPokemon) {
		std::string prefix = logger.getPrefix();

		std::vector<cv::Mat> imagesBGRA =
			opponentPokemonCropper.crop(gameplayBGRA);
		std::vector<cv::Mat> imagesBGR =
			opponentPokemonCropper.crop(gameplayBGR);
		std::vector<cv::Mat> masks =
			opponentPokemonCropper.generateMask(imagesBGR);
		std::vector<cv::Mat> resultsBGRA =
			opponentPokemonCropper.generateTransparentImages(
				imagesBGRA, masks);
		for (int i = 0; i < N_POKEMONS; i++) {
			logger.writeOpponentPokemonImage(prefix, i,
							 resultsBGRA[i]);
		}
		dispatchOpponentTeamShown(resultsBGRA);

		mySelectionOrderMap.fill(0);
		std::vector<std::string> pokemonNames(N_POKEMONS);
		for (int i = 0; i < N_POKEMONS; i++) {
			pokemonNames[i] = pokemonRecognizer.recognizePokemon(
				resultsBGRA[i]);
		}
		logger.writeOpponentTeamText(logger.getPrefix(), pokemonNames);
	}
}

bool ActionHandler::detectSelectionOrderChange(
	const cv::Mat &gameplayBGR, const cv::Mat &gameplayGray,
	std::array<int, N_POKEMONS> &mySelectionOrderMap) const
{
	std::vector<cv::Mat> imagesBGR =
		selectionOrderCropper.crop(gameplayBGR);
	std::vector<cv::Mat> imagesGray =
		selectionOrderCropper.crop(gameplayGray);
	std::array<int, N_POKEMONS> orders;
	bool change_detected = false;
	for (int i = 0; i < N_POKEMONS; i++) {
		orders[i] = selectionRecognizer(imagesBGR[i], imagesGray[i]);
		if (orders[i] > 0 &&
		    mySelectionOrderMap[orders[i] - 1] != i + 1) {
			mySelectionOrderMap[orders[i] - 1] = i + 1;
			change_detected = true;
		}
	}
	if (change_detected) {
		blog(LOG_INFO, "My order: %d %d %d %d %d %d", orders[0],
		     orders[1], orders[2], orders[3], orders[4], orders[5]);
	}
	return change_detected;
}

void ActionHandler::drawMyPokemons(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayHSV,
	std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA,
	const std::array<int, N_POKEMONS> &mySelectionOrderMap) const
{
	std::vector<std::string> imageUrls(N_POKEMONS);
	const std::vector<cv::Mat> croppedBGRA =
		myPokemonCropper.crop(gameplayBGRA);
	const std::vector<bool> shouldUpdate =
		myPokemonCropper.getShouldUpdate(gameplayHSV);

	for (int i = 0; i < N_POKEMONS; i++) {
		if (shouldUpdate[i]) {
			blog(LOG_INFO, "shouldUpdate: %d", i);
			myPokemonsBGRA[i] = croppedBGRA[i].clone();
		}
	}

	for (int i = 0; i < N_POKEMONS; i++) {
		if (myPokemonsBGRA[i].empty()) {
			imageUrls[i] = "";
		} else {
			std::vector<uchar> pngImage;
			cv::imencode(".png", myPokemonsBGRA[i], pngImage);
			imageUrls[i] = "data:image/png;base64," +
				       Base64::encode(pngImage);
		}
	}

	nlohmann::json json{
		{"imageUrls", imageUrls},
		{"mySelectionOrderMap", mySelectionOrderMap},
	};
	const char eventName[] = "obsPokemonSvScreenBuilderMySelectionChanged";
	std::string jsonString = json.dump();
	sendEventToAllBrowserSources(eventName, jsonString.c_str());
}

void ActionHandler::handleSelectPokemon(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
	const cv::Mat &gameplayHsv, const cv::Mat &gameplayGray,
	std::array<int, N_POKEMONS> &mySelectionOrderMap,
	std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA) const
{
	if (detectSelectionOrderChange(gameplayBGR, gameplayGray,
				       mySelectionOrderMap)) {
		drawMyPokemons(gameplayBGRA, gameplayHsv, myPokemonsBGRA,
			       mySelectionOrderMap);
	}
}

void ActionHandler::handleEnteringMatch(bool canEnterToMatch) const
{
	if (canEnterToMatch) {
		nlohmann::json json{{"durationMins", 20}};
		std::string jsonString(json.dump());
		sendEventToAllBrowserSources(
			"obsPokemonSvScreenBuilderMatchStarted",
			jsonString.c_str());
	}
}
