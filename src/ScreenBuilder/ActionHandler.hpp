#pragma once

#include "modules/Logger.hpp"
#include "modules/OpponentRankExtractor.h"
#include "modules/MyRankExtractor.h"

class ActionHandler {
public:
	const MyRankExtractor &myRankExtractor;
	const OpponentRankExtractor &opponentRankExtractor;
	const Logger &logger;

	ActionHandler(const MyRankExtractor &_myRankExtractor,
		      const OpponentRankExtractor &_opponentRankExtractor,
			  const Logger &_logger)
		: myRankExtractor(_myRankExtractor),
		  opponentRankExtractor(_opponentRankExtractor),
		  logger(_logger)
	{
	}

	void handleEnteringRankShown(const cv::Mat &gameplayGray) const;
	void handleEnteringSelectPokemon(const cv::Mat &gameplayGray) const;
};
