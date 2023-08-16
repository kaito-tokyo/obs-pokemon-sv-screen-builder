#include <nlohmann/json.hpp>

#include <obs.h>

#include "plugin-support.h"

#include "ActionHandler.hpp"
#include "TextRecognizer/TextRecognizer.h"
#include "obs-browser-api.h"
#include "Base64/Base64.hpp"

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

	cv::Rect myRankRect = myRankExtractor(gameplayGray);
	if (myRankRect.empty()) {
		obs_log(LOG_INFO, "Failed to extract my rank!");
		logger.writeScreenshot(prefix, "MyRankFailed", gameplayGray);
	} else {
		cv::Mat rankGray = gameplayGray(myRankRect), rankBinary;
		cv::threshold(rankGray, rankBinary, 200, 255,
			      cv::THRESH_BINARY_INV);
		logger.writeMyRankImage(prefix, rankBinary);

		std::string rankText = recognizeText(rankBinary);
		obs_log(LOG_INFO, "My rank text is %s.", rankText.c_str());

		dispatchMyRankShown(rankText);
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

		std::string rankText = recognizeText(rankBinary);
		obs_log(LOG_INFO, "The opponent rank text is %s.",
			rankText.c_str());

		dispatchOpponentRankShown(rankText);
	}
}

void ActionHandler::handleEnteringSelectPokemon(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayBGR,
	bool canEnterToSelectPokemon,
	std::vector<int> &mySelectionOrderMap) const
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
		for (size_t i = 0; i < resultsBGRA.size(); i++) {
			logger.writeOpponentPokemonImage(
				prefix, static_cast<int>(i), resultsBGRA[i]);
		}
		dispatchOpponentTeamShown(resultsBGRA);

		for (size_t i = 0; i < mySelectionOrderMap.size(); i++) {
			mySelectionOrderMap[i] = 0;
		}
		std::vector<std::string> pokemonNames(resultsBGRA.size());
		for (size_t i = 0; i < pokemonNames.size(); i++) {
			pokemonNames[i] = pokemonRecognizer.recognizePokemon(
				resultsBGRA[i]);
		}
		logger.writeOpponentTeamText(logger.getPrefix(), pokemonNames);
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
		orders[i] = selectionRecognizer(imagesBGR[i], imagesGray[i]);
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

void ActionHandler::drawMyPokemons(
	const cv::Mat &gameplayBGRA, const cv::Mat &gameplayHSV,
	std::vector<cv::Mat> &myPokemonsBGRA,
	const std::vector<int> &mySelectionOrderMap) const
{
	const std::vector<cv::Mat> croppedBGRA =
		myPokemonCropper.crop(gameplayBGRA);
	const std::vector<bool> shouldUpdate =
		myPokemonCropper.getShouldUpdate(gameplayHSV);

	for (size_t i = 0; i < myPokemonsBGRA.size(); i++) {
		if (shouldUpdate[i]) {
			blog(LOG_INFO, "shouldUpdate: %lu", i);
			myPokemonsBGRA[i] = croppedBGRA[i].clone();
		}
	}

	std::vector<std::string> imageUrls(myPokemonsBGRA.size());
	for (size_t i = 0; i < myPokemonsBGRA.size(); i++) {
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
	std::vector<int> &mySelectionOrderMap,
	std::vector<cv::Mat> &myPokemonsBGRA) const
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
