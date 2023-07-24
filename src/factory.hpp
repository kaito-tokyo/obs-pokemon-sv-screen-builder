#pragma once

#include <stdexcept>

#include "modules/PokemonRecognizer.h"
#include "Croppers/MyPokemonCropper.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

namespace factory {

class PresetFileNotFoundError : public std::runtime_error {
public:
	PresetFileNotFoundError(const char *name);
};

PokemonRecognizer newPokemonRecognizer(const char *name);
MyPokemonCropper newMyPokemonCropper(const char *name);
HistClassifier newHistClassifier(const char *name);
TemplateClassifier newTemplateClassifier(const char *name);

}
