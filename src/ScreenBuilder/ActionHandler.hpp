#pragma once

#include <nlohmann/json.hpp>

#include "Base64/Base64.hpp"

#include "Croppers/MyPokemonCropper.hpp"
#include "Croppers/OpponentPokemonCropper.hpp"
#include "Croppers/ResultCropper.hpp"
#include "Croppers/SelectionOrderCropper.hpp"
#include "Extractors/MyRankExtractor.hpp"
#include "Extractors/OpponentRankExtractor.hpp"
#include "Recognizers/MyPokemonNameRecognizer.hpp"
#include "Recognizers/MyToolNameRecognizer.hpp"
#include "Recognizers/PokemonRecognizer.hpp"
#include "Recognizers/ResultRecognizer.hpp"
#include "Recognizers/SelectionRecognizer.hpp"

#include "Logger.hpp"
#include "ObsBrowserUtil.hpp"

class ActionHandler {
public:
	ActionHandler(const MyPokemonCropper &_myPokemonCropper,
		      const OpponentPokemonCropper &_opponentPokemonCropper,
		      const ResultCropper &_resultCropper,
		      const SelectionOrderCropper &_selectionOrderCropper,
		      const MyRankExtractor &_myRankExtractor,
		      const OpponentRankExtractor &_opponentRankExtractor,
		      const MyPokemonNameRecognizer &_myPokemonNameRecognizer,
		      const MyToolNameRecognizer &_myToolNameRecognizer,
		      const PokemonRecognizer &_pokemonRecognizer,
		      const ResultRecognizer &_resultRecognizer,
		      const SelectionRecognizer &_selectionRecognizer,
		      const Logger &_logger)
		: myPokemonCropper(_myPokemonCropper),
		  opponentPokemonCropper(_opponentPokemonCropper),
		  resultCropper(_resultCropper),
		  selectionOrderCropper(_selectionOrderCropper),
		  myRankExtractor(_myRankExtractor),
		  opponentRankExtractor(_opponentRankExtractor),
		  myPokemonNameRecognizer(_myPokemonNameRecognizer),
		  myToolNameRecognizer(_myToolNameRecognizer),
		  pokemonRecognizer(_pokemonRecognizer),
		  resultRecognizer(_resultRecognizer),
		  selectionRecognizer(_selectionRecognizer),
		  logger(_logger)
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
	void handleResult(const cv::Mat &gameplayHSV) const;

private:
	const MyPokemonCropper &myPokemonCropper;
	const OpponentPokemonCropper &opponentPokemonCropper;
	const ResultCropper &resultCropper;
	const SelectionOrderCropper &selectionOrderCropper;
	const MyRankExtractor &myRankExtractor;
	const OpponentRankExtractor &opponentRankExtractor;
	const MyPokemonNameRecognizer &myPokemonNameRecognizer;
	const MyToolNameRecognizer &myToolNameRecognizer;
	const PokemonRecognizer &pokemonRecognizer;
	const ResultRecognizer &resultRecognizer;
	const SelectionRecognizer &selectionRecognizer;
	const Logger &logger;

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

	void dispatchOpponentTeamShown(
		const std::vector<cv::Mat> &images,
		const std::vector<std::string> &pokemonNames) const
	{
		std::string prefix = logger.getPrefix();

		for (size_t i = 0; i < images.size(); i++) {
			logger.writeOpponentPokemonImage(
				prefix, static_cast<int>(i), images[i]);
		}

		std::vector<std::string> imageUrls;
		for (size_t i = 0; i < images.size(); i++) {
			std::vector<uchar> pngImage;
			cv::imencode(".png", images[i], pngImage);
			imageUrls.push_back("data:image/png;base64," +
					    Base64::encode(pngImage));
		}

		nlohmann::json json{
			{"imageUrls", imageUrls},
			{"pokemonNames", pokemonNames},
		};
		std::string jsonString = json.dump();

		const char eventName[] =
			"obsPokemonSvScreenBuilderOpponentTeamShown";
		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		nlohmann::json jsonForLog{{"pokemonNames", pokemonNames}};
		std::string jsonStringForLog = jsonForLog.dump();
		logger.writeEvent(prefix, eventName, jsonStringForLog);
	}

	void dispatchMySelectionChanged(
		const std::vector<cv::Mat> &myPokemonImagesBGRA,
		const std::vector<int> &mySelectionOrderMap) const
	{
		std::vector<std::string> imageUrls(myPokemonImagesBGRA.size());
		for (size_t i = 0; i < myPokemonImagesBGRA.size(); i++) {
			if (myPokemonImagesBGRA.at(i).empty()) {
				imageUrls[i] = "";
			} else {
				std::vector<uchar> pngImage;
				cv::imencode(".png", myPokemonImagesBGRA.at(i),
					     pngImage);
				imageUrls[i] = "data:image/png;base64," +
					       Base64::encode(pngImage);
			}
		}

		std::vector<std::string> myPokemonNames(
			myPokemonImagesBGRA.size()),
			myToolNames(myPokemonImagesBGRA.size());
		for (size_t i = 0; i < myPokemonImagesBGRA.size(); i++) {
			if (myPokemonImagesBGRA.at(i).empty()) {
				continue;
			}

			cv::Mat imageGray;
			cv::cvtColor(myPokemonImagesBGRA.at(i), imageGray,
				     cv::COLOR_BGRA2GRAY);
			myPokemonNames.at(i) =
				myPokemonNameRecognizer(imageGray);
			myToolNames.at(i) = myToolNameRecognizer(imageGray);
		}

		nlohmann::json json{
			{"imageUrls", imageUrls},
			{"mySelectionOrderMap", mySelectionOrderMap},
			{"myPokemonNames", myPokemonNames},
			{"myToolNames", myToolNames},
		};
		const char eventName[] =
			"obsPokemonSvScreenBuilderMySelectionChanged";
		std::string jsonString = json.dump();
		sendEventToAllBrowserSources(eventName, jsonString.c_str());

		auto prefix = logger.getPrefix();
		json.erase("imageUrls");
		std::string jsonStringForLog = json.dump();
		logger.writeEvent(prefix, eventName, jsonStringForLog);

		for (size_t i = 0; i < myPokemonImagesBGRA.size(); i++) {
			logger.writeMyPokemonImage(prefix, i,
						   myPokemonImagesBGRA.at(i));
		}
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

	void dispatchMatchCompleted(std::string prefix,
				    std::string resultString) const
	{
		nlohmann::json json{{"resultString", resultString}};
		std::string jsonString(json.dump());

		const char eventName[] =
			"obsPokemonSvScreenBuilderMatchCompleted";
		sendEventToAllBrowserSources(eventName, jsonString.c_str());
		logger.writeEvent(prefix, eventName, jsonString);
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
