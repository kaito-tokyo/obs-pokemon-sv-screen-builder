#include <nlohmann/json.hpp>

#include <obs.h>

#include "plugin-support.h"

#include "ActionHandler.hpp"
#include "modules/TextRecognizer.h"
#include "obs-browser-api.h"

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
		logger.writeScreenshot(prefix, "OpponentRankFailed", gameplayBinary);
	} else {
		cv::Mat rankImage = ~gameplayBinary(opponentRankRect);
		logger.writeOpponentRankImage(prefix, rankImage);

		std::string rankText = recognizeText(rankImage);
		obs_log(LOG_INFO, "The opponent rank text is %s.",
			rankText.c_str());

		dispatchOpponentRankShown(rankText);
	}
}
