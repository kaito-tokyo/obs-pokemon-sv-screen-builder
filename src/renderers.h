#pragma once

#include <nlohmann/json.hpp>

#include "modules/Base64.hpp"
#include "modules/EntityCropper.h"
#include "modules/SelectionRecognizer.h"
#include "modules/Logger.hpp"
#include "Croppers/MyPokemonCropper.hpp"

#include "constants.h"
#include "obs-browser-api.h"

static void renderOpponentPokemons(const cv::Mat &gameplayBGRA,
				   EntityCropper &opponentPokemonCropper,
				   const Logger &logger)
{
	opponentPokemonCropper.crop(gameplayBGRA);
	opponentPokemonCropper.generateMask();
	std::vector<std::string> imageUrls(N_POKEMONS);
	std::string prefix = logger.getPrefix();
	for (int i = 0; i < N_POKEMONS; i++) {
		cv::Mat &image = opponentPokemonCropper.imagesBGRA[i];
		logger.writeOpponentPokemonImage(prefix, i, image);

		std::vector<uchar> pngImage;
		cv::imencode(".png", image, pngImage);
		imageUrls[i] =
			"data:image/png;base64," + Base64::encode(pngImage);
	}
	nlohmann::json json{
		{"imageUrls", imageUrls},
	};
	const char eventName[] = "obsPokemonSvScreenBuilderOpponentTeamShown";
	std::string jsonString = json.dump();
	sendEventToAllBrowserSources(eventName, jsonString.c_str());
}

static bool
detectSelectionOrderChange(EntityCropper &selectionOrderCropper,
			   const cv::Mat &gameplayBGRA,
			   const SelectionRecognizer &selectionRecognizer,
			   std::array<int, N_POKEMONS> &mySelectionOrderMap)
{
	selectionOrderCropper.crop(gameplayBGRA);

	std::array<int, N_POKEMONS> orders;
	bool change_detected = false;
	for (int i = 0; i < N_POKEMONS; i++) {
		orders[i] = selectionRecognizer.recognizeSelection(
			selectionOrderCropper.imagesBGR[i]);
		if (orders[i] > 0 &&
		    mySelectionOrderMap[orders[i] - 1] != i + 1) {
			mySelectionOrderMap[orders[i] - 1] = i + 1;
			change_detected = true;
		}
	}
	if (change_detected) {
		blog(LOG_INFO, "My order: %d %d %d %d %d %d\n", orders[0],
		     orders[1], orders[2], orders[3], orders[4], orders[5]);
	}
	return change_detected;
}

static void
drawMyPokemons(const MyPokemonCropper &myPokemonCropper,
	       const cv::Mat &gameplayBGRA, const cv::Mat &gameplayHSV,
	       std::array<cv::Mat, N_POKEMONS> &myPokemonsBGRA,
	       const std::array<int, N_POKEMONS> &mySelectionOrderMap)
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
