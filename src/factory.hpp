#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <obs-module.h>

#include "modules/PokemonRecognizer.h"

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
	if (moduleFileDstr) {
		std::filesystem::path moduleFilePath = moduleFileDstr;
		bfree(moduleFileDstr);
		return moduleFilePath;
	}

	return std::filesystem::path();
}

static PokemonRecognizer newPokemonRecognizer(void)
{
	auto path = getConfigPath("config/PokemonRecognizer.cbor");
	if (path.empty()) {
		throw;
	}

	std::ifstream ifs(path);
	nlohmann::json json = nlohmann::json::from_cbor(ifs);
	return {
		json["height"].template get<int>(),
		json["descriptorSize"].template get<int>(),
		json["data"]
			.template get<std::vector<std::vector<unsigned char>>>(),
		json["pokemonNames"].template get<std::vector<std::string>>(),
	};
}
}
