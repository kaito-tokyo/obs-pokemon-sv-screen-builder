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

template<> struct adl_serializer<cv::Scalar> {
	static void from_json(const json &j, cv::Scalar &opt)
	{
		auto list = j.template get<std::vector<double>>();
		opt = {list[0], list[1], list[2], list[3]};
	}
};
} // namespace nlohmann

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

PresetFileNotFoundError::PresetFileNotFoundError(const char *name)
	: std::runtime_error(generateWhatString(name))
{
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

OpponentPokemonCropper newOpponentPokemonCropper(const char *name)
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
		json["seed"].template get<cv::Point>(),
		json["loDiff"].template get<cv::Scalar>(),
		json["upDiff"].template get<cv::Scalar>(),
	};
}

ResultCropper newResultCropper(const char *name)
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
	};
}

SelectionOrderCropper newSelectionOrderCropper(const char *name)
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
	};
}

MyRankExtractor newMyRankExtractor(const char *name)
{
	auto path = getPresetPath(name);
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);

	return {
		json["rects"].template get<std::vector<cv::Rect>>(),
		json["threshold"].template get<int>(),
		json["cols"].template get<std::vector<int>>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
		json["ratio"].template get<double>(),
		json["matchTypes"].template get<std::vector<std::string>>(),
	};
}

OpponentRankExtractor newOpponentRankExtractor(const char *name)
{
	auto path = getPresetPath(name);
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);

	return {
		json["rect"].template get<cv::Rect>(),
		json["threshold"].template get<int>(),
		json["parenMap"]
			.template get<
				std::map<std::string, std::pair<int, int>>>(),
		json["cols"].template get<std::vector<int>>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
		json["ratio"].template get<double>(),
	};
}

MyPokemonNameRecognizer newMyPokemonNameRecognizer(const char *name)
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
		json["thresh"].template get<double>(),
	};
}

MySelectionRecognizer newMySelectionRecognizer(const char *name)
{
	fs::path path = getPresetPath(name);
	if (path.empty()) {
		throw PresetFileNotFoundError(name);
	}
	std::ifstream ifs(path, std::ios_base::binary);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);
	return {
		json["blueThreshold"].template get<int>(),
		json["binaryThreshold"].template get<int>(),
		json["ratio"].template get<double>(),
		json["indices"].template get<std::vector<int>>(),
		json["cols"].template get<std::vector<int>>(),
		json["data"].template get<std::vector<std::vector<uchar>>>(),
	};
}

MyToolNameRecognizer newMyToolNameRecognizer(const char *name)
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
		json["thresh"].template get<double>(),
	};
}

OpponentPokemonImageRecognizer
newOpponentPokemonImageRecognizer(const char *name)
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

ResultRecognizer newResultRecognizer(const char *name)
{
	fs::path path = getPresetPath(name);
	if (path.empty()) {
		throw PresetFileNotFoundError(name);
	}
	std::ifstream ifs(path);
	nlohmann::json json;
	ifs >> json;

	return {
		json["nBins"].template get<int>(),
		json["winMaxIndex"].template get<int>(),
		json["winRatio"].template get<double>(),
		json["loseMaxIndex"].template get<int>(),
		json["loseRatio"].template get<double>(),
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
		name,
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

MatchStateAggregator newMatchStateAggregator(const char *name)
{
	auto path = getPresetPath(name);
	std::ifstream ifs(path);
	nlohmann::json json;
	ifs >> json;

	std::regex matchStateRegex(
		json["matchStateRegex"].template get<std::string>());
	return {
		matchStateRegex,
		json["columnNames"].template get<std::vector<std::string>>(),
	};
}

} // namespace factory
