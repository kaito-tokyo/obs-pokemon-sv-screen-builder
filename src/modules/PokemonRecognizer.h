#pragma once

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <obs.h>
#include <iostream>

class PokemonRecognizer {
private:
	static const std::vector<cv::Mat>
	generateDescriptors(int cols,
			    std::vector<std::vector<unsigned char>> &data)
	{
		std::vector<cv::Mat> descriptors(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			descriptors[i] =
				cv::Mat(static_cast<int>(data[i].size()) / cols,
					cols, CV_8U, data[i].data());
		}
		return descriptors;
	}

public:
	PokemonRecognizer(int height, int descriptorSize,
			  std::vector<std::vector<unsigned char>> data,
			  std::vector<std::string> pokemonNames)
		: HEIGHT(height),
		  DESCRIPTOR_SIZE(descriptorSize),
		  DATA(data),
		  POKEMON_NAMES(pokemonNames),
		  DESCRIPTORS(
			  generateDescriptors((DESCRIPTOR_SIZE + 7) / 8, DATA))
	{
	}

	std::string recognizePokemon(const cv::Mat &imageBGRA) const;

private:
	const int HEIGHT;
	const int DESCRIPTOR_SIZE;
	std::vector<std::vector<unsigned char>> DATA;
	const std::vector<std::string> POKEMON_NAMES;
	const std::vector<cv::Mat> DESCRIPTORS;
};
