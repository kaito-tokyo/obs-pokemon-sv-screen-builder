#pragma once

#include <stdexcept>

#include "Croppers/MyPokemonCropper.hpp"
#include "Croppers/OpponentPokemonCropper.hpp"
#include "Croppers/ResultCropper.hpp"
#include "Croppers/SelectionOrderCropper.hpp"
#include "Extractors/MyRankExtractor.hpp"
#include "Extractors/OpponentRankExtractor.hpp"
#include "Recognizers/MyPokemonNameRecognizer.hpp"
#include "Recognizers/MySelectionRecognizer.hpp"
#include "Recognizers/MyToolNameRecognizer.hpp"
#include "Recognizers/OpponentPokemonImageRecognizer.hpp"
#include "Recognizers/ResultRecognizer.hpp"
#include "ScreenBuilder/HistClassifier.hpp"
#include "ScreenBuilder/TemplateClassifier.hpp"
#include "ScreenBuilder/MatchStateAggregator.hpp"

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
MyPokemonNameRecognizer newMyPokemonNameRecognizer(const char *name);
MySelectionRecognizer newMySelectionRecognizer(const char *name);
MyToolNameRecognizer newMyToolNameRecognizer(const char *name);
OpponentPokemonImageRecognizer
newOpponentPokemonImageRecognizer(const char *name);
ResultRecognizer newResultRecognizer(const char *name);
HistClassifier newHistClassifier(const char *name);
TemplateClassifier newTemplateClassifier(const char *name);
MatchStateAggregator newMatchStateAggregator(const char *name);

} // namespace factory
