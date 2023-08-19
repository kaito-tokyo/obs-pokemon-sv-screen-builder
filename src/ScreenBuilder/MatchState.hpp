#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

struct MatchState {
	std::string timestamp = "";
	std::string myRank = "";
	std::string opponentRank = "";
	std::vector<std::string> myPokemonNames = std::vector<std::string>(6);
	std::vector<std::string> myToolNames = std::vector<std::string>(6);
	std::vector<int> mySelectionMap = std::vector<int>(6);
	std::vector<std::string> opponentPokemonIds =
		std::vector<std::string>(6);
	std::string resultString = "";

	std::string toJSONString(void) const
	{
		std::vector<std::string> mySelectionPokemonNames(
			mySelectionMap.size()),
			mySelectionToolNames(mySelectionMap.size());
		for (size_t i = 0; i < mySelectionMap.size(); i++) {
			int index = mySelectionMap.at(i);
			if (index > 0) {
				mySelectionPokemonNames.at(i) =
					myPokemonNames.at(index - 1);
				mySelectionToolNames.at(i) =
					myToolNames.at(index - 1);
			}
		}

		nlohmann::json json{
			{"timestamp", timestamp},
			{"myRank", myRank},
			{"opponentRank", opponentRank},
			{"myPokemonName1", myPokemonNames.at(0)},
			{"myPokemonName2", myPokemonNames.at(1)},
			{"myPokemonName3", myPokemonNames.at(2)},
			{"myPokemonName4", myPokemonNames.at(3)},
			{"myPokemonName5", myPokemonNames.at(4)},
			{"myPokemonName6", myPokemonNames.at(5)},
			{"myToolName1", myToolNames.at(0)},
			{"myToolName2", myToolNames.at(1)},
			{"myToolName3", myToolNames.at(2)},
			{"myToolName4", myToolNames.at(3)},
			{"myToolName5", myToolNames.at(4)},
			{"myToolName6", myToolNames.at(5)},
			{"mySelectionPokemonName1",
			 mySelectionPokemonNames.at(0)},
			{"mySelectionPokemonName2",
			 mySelectionPokemonNames.at(1)},
			{"mySelectionPokemonName3",
			 mySelectionPokemonNames.at(2)},
			{"mySelectionPokemonName4",
			 mySelectionPokemonNames.at(3)},
			{"mySelectionPokemonName5",
			 mySelectionPokemonNames.at(4)},
			{"mySelectionPokemonName6",
			 mySelectionPokemonNames.at(5)},
			{"mySelectionToolName1", mySelectionToolNames.at(0)},
			{"mySelectionToolName2", mySelectionToolNames.at(1)},
			{"mySelectionToolName3", mySelectionToolNames.at(2)},
			{"mySelectionToolName4", mySelectionToolNames.at(3)},
			{"mySelectionToolName5", mySelectionToolNames.at(4)},
			{"mySelectionToolName6", mySelectionToolNames.at(5)},
			{"opponentPokemonId1", opponentPokemonIds.at(0)},
			{"opponentPokemonId2", opponentPokemonIds.at(1)},
			{"opponentPokemonId3", opponentPokemonIds.at(2)},
			{"opponentPokemonId4", opponentPokemonIds.at(3)},
			{"opponentPokemonId5", opponentPokemonIds.at(4)},
			{"opponentPokemonId6", opponentPokemonIds.at(5)},
			{"resultString", resultString},
		};

		return json.dump();
	}
};
