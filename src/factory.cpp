#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include <obs-module.h>

#include "factory.hpp"

namespace fs = std::filesystem;

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

static fs::path getPresetPath(const char *name)
{
    char *moduleConfigDstr = obs_module_config_path(name);
    if (moduleConfigDstr) {
        fs::path moduleConfigPath(moduleConfigDstr);
        bfree(moduleConfigDstr);
        if (fs::exists(moduleConfigPath)) {
            return moduleConfigPath;
        }
    }

	char *moduleFileDstr = obs_module_file(name);
    if (moduleFileDstr) {
        fs::path moduleFilePath(moduleFileDstr);
        bfree(moduleFileDstr);
        return moduleFilePath;
    }

    return {};
}

namespace factory {

static std::string generateWhatString(const char *name)
{
    return "Preset file " + std::string(name) + " was not found!";
}

PresetFileNotFoundError::PresetFileNotFoundError(const char *name) : std::runtime_error(generateWhatString(name)) {}

PokemonRecognizer newPokemonRecognizer(const char *name)
{
	fs::path path = getPresetPath(name);
    if (path.empty()) {
        throw PresetFileNotFoundError(name);
    }
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);
	return {
		json["height"].template get<int>(),
		json["descriptorSize"].template get<int>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
		json["pokemonNames"].template get<std::vector<std::string>>(),
	};
}

MyPokemonCropper newMyPokemonCropper(const char *name)
{
	fs::path path = getPresetPath(name);
    if (path.empty()) {
        throw PresetFileNotFoundError(name);
    }
	std::ifstream ifs(path);
	nlohmann::json json;
	ifs >> json;
	return {
		json["rects"].template get<std::vector<cv::Rect>>(),
		json["backgroundValueThreshold"].template get<uchar>(),
		json["backgroundPoint"].template get<cv::Point>(),
	};
}

HistClassifier newHistClassifier(const char *name)
{
	fs::path path = getPresetPath(name);
    if (path.empty()) {
        throw PresetFileNotFoundError(name);
    }
	std::ifstream ifs(path);
	nlohmann::json json;
	ifs >> json;

	return {
		json["rect"].template get<cv::Rect>(),
		json["channel"].template get<int>(),
		json["nBins"].template get<int>(),
		json["maxIndex"].template get<int>(),
		json["ratio"].template get<double>(),
	};
}

TemplateClassifier newTemplateClassifier(const char *name)
{
	auto path = getPresetPath(name);
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);

	return {
		json["rect"].template get<cv::Rect>(),
		json["threshold"].template get<int>(),
		json["ratio"].template get<double>(),
		json["cols"].template get<std::vector<int>>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
	};
}
}
