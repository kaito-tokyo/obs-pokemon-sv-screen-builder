#include <ctime>
#include <iomanip>
#include <sstream>

#include "Logger.hpp"

std::string Logger::getPrefix(void) const
{
	std::ostringstream oss;
	std::time_t now = std::time(nullptr);
	std::tm *localTime = std::localtime(&now);
	oss << std::put_time(localTime, "%Y%m%dT%H%M%S");
	return oss.str();
}

void Logger::writeOpponentPokemonImage(const std::string &prefix, int index,
				       const cv::Mat &image) const
{
	if (basedir.empty()) {
		return;
	}
	std::ostringstream oss;
	oss << prefix << "-OpponentPokemon-" << index << ".png";
	const std::string filename = oss.str();
	writeImage(oss.str(), image);
}

void Logger::writeOpponentRankImage(const std::string &prefix,
				    const cv::Mat &image) const
{
	if (basedir.empty()) {
		return;
	}
	std::ostringstream oss;
	oss << prefix << "-OpponentRank.png";
	const std::string filename = oss.str();
	writeImage(oss.str(), image);
}

void Logger::writeMyPokemonImage(const std::string &prefix, int index,
				 const cv::Mat &image) const
{
	if (basedir.empty()) {
		return;
	}
	std::ostringstream oss;
	oss << prefix << "-MyPokemon-" << index << ".png";
	const std::string filename = oss.str();
	writeImage(oss.str(), image);
}

void Logger::writeImage(const std::string &filename, const cv::Mat &image) const
{
	std::filesystem::create_directories(basedir);
	std::filesystem::path p(basedir);
	p /= filename;
	cv::imwrite(p.string<char>(), image);
}