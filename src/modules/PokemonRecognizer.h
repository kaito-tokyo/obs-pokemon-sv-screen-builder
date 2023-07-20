#pragma once

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

class PokemonRecognizer {
public:
	std::string recognizePokemon(const cv::Mat &imageBGRA) const;

private:
	static const int HEIGHT;
	static const int DESCRIPTOR_SIZE;
	static const std::vector<std::string> POKEMON_NAMES;
	static const std::vector<cv::Mat> POKEMON_DESCRIPTORS;
};
