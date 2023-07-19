#pragma once

#include <string>
#include <filesystem>

#include <opencv2/opencv.hpp>

class Logger {
public:
	std::filesystem::path basedir;
	std::string getPrefix(void) const;
	void writeOpponentPokemonImage(const std::string &prefix, int index,
				       const cv::Mat &image) const;
	void writeOpponentRankImage(const std::string &prefix,
				    const cv::Mat &image) const;
	void writeMyPokemonImage(const std::string &prefix, int index,
				 const cv::Mat &image) const;

private:
	void writeImage(const std::string &filename,
			const cv::Mat &image) const;
};
