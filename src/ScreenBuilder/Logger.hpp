#pragma once

#include <string>
#include <filesystem>
#include <fstream>

#include <opencv2/opencv.hpp>

class Logger {
public:
	std::filesystem::path basedir;

	std::string getPrefix(void) const
	{
		std::ostringstream oss;
		std::time_t now = std::time(nullptr);
		std::tm *localTime = std::localtime(&now);
		oss << std::put_time(localTime, "%Y%m%dT%H%M%S");
		return oss.str();
	}

	void writeOpponentPokemonImage(const std::string &prefix, int index,
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

	void writeOpponentRankImage(const std::string &prefix,
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

	void writeMyRankImage(const std::string &prefix,
			      const cv::Mat &image) const
	{
		if (basedir.empty()) {
			return;
		}
		std::ostringstream oss;
		oss << prefix << "-MyRank.png";
		const std::string filename = oss.str();
		writeImage(oss.str(), image);
	}

	void writeMyPokemonImage(const std::string &prefix, int index,
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

	void writeScreenshot(const std::string &prefix, const std::string &name,
			     const cv::Mat &image) const
	{
		if (basedir.empty()) {
			return;
		}
		std::ostringstream oss;
		oss << prefix << "-" << name << ".png";
		const std::string filename = oss.str();
		writeImage(oss.str(), image);
	}

	void writeEvent(const std::string &prefix, const std::string &eventName,
			const std::string &jsonString) const
	{
		if (basedir.empty()) {
			return;
		}
		std::filesystem::path p = basedir;
		p /= prefix + "-" + eventName + ".txt";
		std::ofstream ofs(p);
		ofs << jsonString << std::endl;
	}

private:
	void writeImage(const std::string &filename, const cv::Mat &image) const
	{
		if (image.empty()) {
			return;
		}
		std::filesystem::create_directories(basedir);
		std::filesystem::path p(basedir);
		p /= filename;
		cv::imwrite(p.string<char>(), image);
	}
};
