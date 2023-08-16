#pragma once

#include <nlohmann/json.hpp>

#include "obs-browser-api.h"
#include "Base64/Base64.hpp"
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
	void handleSelectPokemon(const cv::Mat &gameplayBGRA,
				 const cv::Mat &gameplayBGR,
				 const cv::Mat &gameplayHsv,
				 const cv::Mat &gameplayGray,
				 std::vector<int> &mySelectionOrderMap,
				 std::vector<cv::Mat> &myPokemonsBGRA) const;
	void handleEnteringMatch(bool canEnterToMatch) const;

private:
	void dispatchMyRankShown(const std::string &text) const
	{
		nlohmann::json json{{"text", text}};
		std::string jsonString(json.dump());

		const char eventName[] = "obsPokemonSvScreenBuilderMyRankShown";

		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		logger.writeEvent(logger.getPrefix(), eventName, jsonString);
	}

	void dispatchOpponentRankShown(const std::string &text) const
	{
		nlohmann::json json{{"text", text}};
		std::string jsonString(json.dump());

		const char eventName[] =
			"obsPokemonSvScreenBuilderOpponentRankShown";
		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		logger.writeEvent(logger.getPrefix(), eventName, jsonString);
	}

	void dispatchOpponentTeamShown(const std::vector<cv::Mat> &images) const
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

		const char eventName[] =
			"obsPokemonSvScreenBuilderOpponentTeamShown";
		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		logger.writeEvent(logger.getPrefix(), eventName, jsonString);
	}

	void dispatchMySelectionChanged(
		const std::vector<cv::Mat> &images,
		const std::vector<int> &mySelectionOrderMap) const
	{
		std::vector<std::string> imageUrls(images.size());
		for (size_t i = 0; i < images.size(); i++) {
			if (images[i].empty()) {
				imageUrls[i] = "";
			} else {
				std::vector<uchar> pngImage;
				cv::imencode(".png", images[i], pngImage);
				imageUrls[i] = "data:image/png;base64," +
					       Base64::encode(pngImage);
			}
		}

		nlohmann::json json{
			{"imageUrls", imageUrls},
			{"mySelectionOrderMap", mySelectionOrderMap},
		};
		const char eventName[] =
			"obsPokemonSvScreenBuilderMySelectionChanged";
		std::string jsonString = json.dump();
		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		logger.writeEvent(logger.getPrefix(), eventName, jsonString);
	}

	void dispatchMatchStarted(int durationMins) const
	{
		nlohmann::json json{{"durationMins", durationMins}};
		std::string jsonString(json.dump());

		const char eventName[] =
			"obsPokemonSvScreenBuilderMatchStarted";
		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		logger.writeEvent(logger.getPrefix(), eventName, jsonString);
	}

	bool
	detectSelectionOrderChange(const cv::Mat &gameplayBGR,
				   const cv::Mat &gameplayGray,
				   std::vector<int> &mySelectionOrderMap) const;
	void drawMyPokemons(const cv::Mat &gameplayBGRA,
			    const cv::Mat &gameplayHSV,
			    std::vector<cv::Mat> &myPokemonsBGRA,
			    const std::vector<int> &mySelectionOrderMap) const;
};
