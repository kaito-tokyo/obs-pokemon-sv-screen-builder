#pragma once

#include <array>
#include <vector>

#include <opencv2/opencv.hpp>

#include "constants.h"

class MyPokemonCropper {
public:
	const cv::Range colRange = {182, 711};
	const std::vector<cv::Range> rowRanges = {{147, 254}, {263, 371},
						  {379, 486}, {496, 602},
						  {612, 718}, {727, 834}};

	std::array<cv::Mat, N_POKEMONS> crop(const cv::Mat &input)
	{
		std::array<cv::Mat, N_POKEMONS> output;
		for (size_t i = 0; i < rowRanges.size(); i++) {
			output[i] = input(rowRanges[i], colRange).clone();
		}
		return output;
	}
};
