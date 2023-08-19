#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>

#include <nlohmann/json.hpp>

#include "plugin-support.h"

class MatchStateAggregator {
public:
	MatchStateAggregator(const std::regex &_matchStateRegex,
			     const std::vector<std::string> &_columnNames)
		: matchStateRegex(_matchStateRegex), columnNames(_columnNames)
	{
	}

	std::filesystem::path
	operator()(const std::string &timestamp,
		   const std::filesystem::path &basedir) const
	{
		namespace fs = std::filesystem;

		const auto matchStatePaths = listMatchStatePaths(basedir);

		auto outputPath = basedir;
		outputPath /= "MatchSheet-";
		outputPath += timestamp;
		outputPath += ".txt";
		std::ofstream ofs(outputPath);
		writeHeader(ofs);
		for (const auto &path : matchStatePaths) {
			std::ifstream ifs(path);
			nlohmann::json json;
			ifs >> json;
			writeLine(ofs, json);
		}

		return outputPath;
	}

private:
	const std::regex matchStateRegex;
	const std::vector<std::string> columnNames;

	std::vector<std::filesystem::path>
	listMatchStatePaths(const std::filesystem::path &basedir) const
	{
		namespace fs = std::filesystem;
		std::vector<fs::path> matchStatePaths;
		for (const auto &entry : fs::directory_iterator(basedir)) {
			if (!isPathMatchState(entry)) {
				continue;
			}
			matchStatePaths.push_back(entry.path());
		}
		std::sort(matchStatePaths.begin(), matchStatePaths.end());
		return matchStatePaths;
	}

	bool
	isPathMatchState(const std::filesystem::directory_entry &entry) const
	{
		if (!entry.is_regular_file()) {
			return false;
		}
		const auto &filename = entry.path().filename().string<char>();
		if (!std::regex_match(filename, matchStateRegex)) {
			return false;
		}
		return true;
	}

	void writeHeader(std::ofstream &ofs) const
	{
		for (const auto &columnName : columnNames) {
			ofs << columnName << "\t";
		}
		ofs << std::endl;
	}

	void writeLine(std::ofstream &ofs, const nlohmann::json &json) const
	{
		for (const auto &columnName : columnNames) {
			const auto &field = json[columnName];
			if (field.is_null()) {
				ofs << "\t";
				continue;
			}
			ofs << field.get<std::string>() << "\t";
		}
		ofs << std::endl;
	}
};
