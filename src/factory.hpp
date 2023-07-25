#pragma once

#include <stdexcept>

#include "Croppers/MyPokemonCropper.hpp"
#include "Recognizers/PokemonRecognizer.h"
#include "Recognizers/SelectionRecognizer.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

namespace factory {

class PresetFileNotFoundError : public std::runtime_error {
public:
	PresetFileNotFoundError(const char *name);
};

PokemonRecognizer newPokemonRecognizer(const char *name);
SelectionRecognizer newSelectionRecognizer(const char *name);
MyPokemonCropper newMyPokemonCropper(const char *name);
HistClassifier newHistClassifier(const char *name);
TemplateClassifier newTemplateClassifier(const char *name);
TemplateClassifier newTemplateClassifier(const char *name);

}
