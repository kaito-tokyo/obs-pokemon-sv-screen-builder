#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include <obs-module.h>

#include "modules/PokemonRecognizer.h"
#include "Croppers/MyPokemonCropper.hpp"

namespace nlohmann {
template<> struct adl_serializer<cv::Rect> {
	static void from_json(const json &j, cv::Rect &opt)
	{
		auto list = j.template get<std::vector<int>>();
		opt = {list[0], list[1], list[2], list[3]};
	}
};

template<> struct adl_serializer<cv::Point> {
	static void from_json(const json &j, cv::Point &opt)
	{
		auto list = j.template get<std::vector<int>>();
		opt = {list[0], list[1]};
	}
};
}

namespace factory {

static std::filesystem::path getConfigPath(const char *file)
{
	char *moduleConfigDstr = obs_module_config_path(file);
	std::filesystem::path moduleConfigPath(moduleConfigDstr);
	bfree(moduleConfigDstr);
	if (std::filesystem::exists(moduleConfigPath)) {
		return moduleConfigPath;
	}

	char *moduleFileDstr = obs_module_file(file);
	std::filesystem::path moduleFilePath = moduleFileDstr;
	bfree(moduleFileDstr);
	return moduleFilePath;
}

static PokemonRecognizer newPokemonRecognizer(void)
{
	auto path = getConfigPath("config/PokemonRecognizer.cbor");
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);
	return {
		json["height"].template get<int>(),
		json["descriptorSize"].template get<int>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
		json["pokemonNames"].template get<std::vector<std::string>>(),
	};
}

static MyPokemonCropper newMyPokemonCropper(void)
{
	auto path = getConfigPath("config/MyPokemonCropper.json");
	std::ifstream ifs(path);
	nlohmann::json json;
	ifs >> json;
	return {
		json["rects"].template get<std::vector<cv::Rect>>(),
		json["backgroundValueThreshold"].template get<uchar>(),
		json["backgroundPoint"].template get<cv::Point>(),
	};
}
}
