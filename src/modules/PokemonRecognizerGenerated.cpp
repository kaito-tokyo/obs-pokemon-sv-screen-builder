#include <vector>
#include <map>
#include <string>
#include <utility>
#include <opencv2/opencv.hpp>
#include "PokemonRecognizer.h"

const int PokemonRecognizer::HEIGHT = 98;
const int PokemonRecognizer::DESCRIPTOR_SIZE = 16;

static std::vector<std::vector<uchar>> DATA = {

	{0,  38, 0,   100, 16, 38, 20, 36, 60, 23, 40,
	 62, 17, 164, 8,   22, 8,  22, 72, 54, 0,  38},

	{8,  38, 22, 23, 8,   22, 124, 30, 0,   237, 88, 127,
	 16, 76, 0,  78, 105, 95, 24,  87, 128, 233, 18, 22},

	{144, 102, 16, 102, 24,  22, 22, 6,  56,  95, 52, 22, 73, 236, 73,
	 228, 48,  78, 89,  204, 0,  78, 81, 204, 8,  38, 8,  54, 16,  4},

	{121, 30, 16, 54, 20, 6,  73, 22, 8,   22,  8,  22, 28, 22,  80, 182,
	 0,   22, 40, 46, 20, 54, 8,  46, 144, 180, 24, 14, 4,  103, 0,  54},

	{8,   22, 20, 180, 16, 70, 20, 69, 8,  127, 20,
	 119, 18, 66, 24,  22, 12, 54, 93, 22, 20,  180},

	{8, 22,  24, 22, 88, 150, 8,  118, 16, 132, 18, 4,   16, 100,
	 0, 247, 52, 79, 49, 205, 65, 196, 16, 70,  0,  119, 65, 196},

};

const std::vector<std::string> PokemonRecognizer::POKEMON_NAMES = {

	"chi-yu",

	"dragapult",

	"baxcalibur",

	"iron-valiant",

	"iron-thorns",

	"gholdengo",

};

const std::vector<cv::Mat> PokemonRecognizer::POKEMON_DESCRIPTORS = {

	cv::Mat(static_cast<int>(DATA[0].size()) / 2, 2, CV_8U, DATA[0].data()),

	cv::Mat(static_cast<int>(DATA[1].size()) / 2, 2, CV_8U, DATA[1].data()),

	cv::Mat(static_cast<int>(DATA[2].size()) / 2, 2, CV_8U, DATA[2].data()),

	cv::Mat(static_cast<int>(DATA[3].size()) / 2, 2, CV_8U, DATA[3].data()),

	cv::Mat(static_cast<int>(DATA[4].size()) / 2, 2, CV_8U, DATA[4].data()),

	cv::Mat(static_cast<int>(DATA[5].size()) / 2, 2, CV_8U, DATA[5].data()),

};
