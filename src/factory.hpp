#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include <obs-module.h>

#include "modules/PokemonRecognizer.h"
#include "Croppers/MyPokemonCropper.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

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

static std::filesystem::path getConfigPath(std::string file)
{
	char *moduleConfigDstr = obs_module_config_path(file.c_str());
	std::filesystem::path moduleConfigPath(moduleConfigDstr);
	bfree(moduleConfigDstr);
	if (std::filesystem::exists(moduleConfigPath)) {
		return moduleConfigPath;
	}

	char *moduleFileDstr = obs_module_file(file.c_str());
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

static HistClassifier newHistClassifier(std::string name)
{
	auto path = getConfigPath("config/" + name);
	std::ifstream ifs(path);
	nlohmann::json json;
	ifs >> json;

	return {
		json["rects"].template get<cv::Rect>(),
		json["channel"].template get<int>(),
		json["nBins"].template get<int>(),
		json["maxIndex"].template get<int>(),
		json["ratio"].template get<double>(),
	};
}

static TemplateClassifier newTemplateClassifier(std::string name)
{
	auto path = getConfigPath("config/" + name);
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);

	return {
		json["rects"].template get<cv::Rect>(),
		json["threshold"].template get<int>(),
		json["ratio"].template get<double>(),
		json["cols"].template get<std::vector<int>>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
	};
}
}
