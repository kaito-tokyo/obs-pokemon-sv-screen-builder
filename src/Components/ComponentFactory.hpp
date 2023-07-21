#pragma once

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include <obs-module.h>

#include "MyPokemonCropper.hpp"

class ComponentFactory {
    static std::filesystem::path getConfigPathIfExists(const char *file)
    {
        std::filesystem::path path = obs_module_config_path(file);
        if (std::filesystem::exists(path)) {
            return path;
        } else {
            return "";
        }
    }

    static MyPokemonCropper newMyPokemonCropper(void) {
        const auto path = getConfigPathIfExists("MyPokemonCropper.json");
        if (path.empty()) {
            return MyPokemonCropper();
        } else {
            std::ifstream ifs(path);
            nlohmann::json j;
            ifs >> j;
            return {
                j[""]
            };
        }
    }
}