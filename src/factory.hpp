#pragma once

#include <stdexcept>

#include "Croppers/MyPokemonCropper.hpp"
#include "Croppers/OpponentPokemonCropper.hpp"
#include "Croppers/ResultCropper.hpp"
#include "Croppers/SelectionOrderCropper.hpp"
#include "Extractors/MyRankExtractor.hpp"
#include "Extractors/OpponentRankExtractor.hpp"
#include "Recognizers/PokemonRecognizer.hpp"
#include "Recognizers/ResultRecognizer.hpp"
#include "Recognizers/SelectionRecognizer.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"

namespace factory {

class PresetFileNotFoundError : public std::runtime_error {
public:
	PresetFileNotFoundError(const char *name);
};

MyPokemonCropper newMyPokemonCropper(const char *name);
OpponentPokemonCropper newOpponentPokemonCropper(const char *name);
ResultCropper newResultCropper(const char *name);
SelectionOrderCropper newSelectionOrderCropper(const char *name);
MyRankExtractor newMyRankExtractor(const char *name);
OpponentRankExtractor newOpponentRankExtractor(const char *name);
PokemonRecognizer newPokemonRecognizer(const char *name);
ResultRecognizer newResultRecognizer(const char *name);
SelectionRecognizer newSelectionRecognizer(const char *name);
HistClassifier newHistClassifier(const char *name);
TemplateClassifier newTemplateClassifier(const char *name);

}
