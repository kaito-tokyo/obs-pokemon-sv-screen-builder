#pragma once

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

class PokemonRecognizer {
public:
	std::string recognizePokemon(const cv::Mat &imageBGR,
				     const cv::Mat &mask) const;

private:
	static const std::vector<std::string> POKEMON_IDS;
	static const std::vector<cv::Mat> POKEMON_DESCRIPTORS;
};
