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
	std::vector<std::string> opponentPokemonNames =
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
			{"myPokemon1", myPokemonNames.at(0)},
			{"myPokemon2", myPokemonNames.at(1)},
			{"myPokemon3", myPokemonNames.at(2)},
			{"myPokemon4", myPokemonNames.at(3)},
			{"myPokemon5", myPokemonNames.at(4)},
			{"myPokemon6", myPokemonNames.at(5)},
			{"myTool1", myToolNames.at(0)},
			{"myTool2", myToolNames.at(1)},
			{"myTool3", myToolNames.at(2)},
			{"myTool4", myToolNames.at(3)},
			{"myTool5", myToolNames.at(4)},
			{"myTool6", myToolNames.at(5)},
			{"mySelectionPokemon1", mySelectionPokemonNames.at(0)},
			{"mySelectionPokemon2", mySelectionPokemonNames.at(1)},
			{"mySelectionPokemon3", mySelectionPokemonNames.at(2)},
			{"mySelectionPokemon4", mySelectionPokemonNames.at(3)},
			{"mySelectionPokemon5", mySelectionPokemonNames.at(4)},
			{"mySelectionPokemon6", mySelectionPokemonNames.at(5)},
			{"mySelectionTool1", mySelectionToolNames.at(0)},
			{"mySelectionTool2", mySelectionToolNames.at(1)},
			{"mySelectionTool3", mySelectionToolNames.at(2)},
			{"mySelectionTool4", mySelectionToolNames.at(3)},
			{"mySelectionTool5", mySelectionToolNames.at(4)},
			{"mySelectionTool6", mySelectionToolNames.at(5)},
			{"opponentPokemon1", opponentPokemonNames.at(0)},
			{"opponentPokemon2", opponentPokemonNames.at(1)},
			{"opponentPokemon3", opponentPokemonNames.at(2)},
			{"opponentPokemon4", opponentPokemonNames.at(3)},
			{"opponentPokemon5", opponentPokemonNames.at(4)},
			{"opponentPokemon6", opponentPokemonNames.at(5)},
			{"resultString", resultString},
		};

		return json.dump();
	}
};
