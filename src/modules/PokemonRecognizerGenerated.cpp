#include <vector>
#include <map>
#include <string>
#include <utility>
#include <opencv2/opencv.hpp>
#include "PokemonRecognizer.h"

const int PokemonRecognizer::HEIGHT = 196;
const int PokemonRecognizer::DESCRIPTOR_SIZE = 16;

static std::vector<std::vector<uchar>> DATA = {

	{125, 95,  72,  54,  0,  4,   154, 86,  91,  2,   16, 86,  16,  22,
	 20,  87,  0,   54,  34, 91,  73,  146, 18,  22,  40, 79,  16,  213,
	 48,  95,  18,  79,  16, 245, 18,  150, 52,  255, 8,  14,  55,  0,
	 16,  38,  32,  127, 9,  8,   9,   2,   150, 99,  25, 79,  77,  182,
	 9,   79,  20,  39,  24, 79,  72,  119, 72,  182, 8,  22,  16,  68,
	 40,  111, 12,  119, 16, 6,   8,   87,  125, 95,  0,  38,  16,  101,
	 8,   38,  60,  31,  8,  22,  52,  132, 18,  132, 69, 229, 146, 4,
	 10,  18,  8,   86,  8,  118, 4,   119, 0,   119, 81, 180, 68,  247,
	 68,  182, 147, 0,   9,  75,  17,  68,  16,  54,  93, 180, 16,  68,
	 18,  132, 16,  22,  0,  100, 0,   38,  0,   38,  20, 36,  60,  7,
	 20,  22,  36,  62,  17, 164, 8,   86,  8,   54,  8,  54,  0,   38},

	{8,   22,  0,   38,  20,  119, 8,   78,  20,  4,   40,  94,  8,  119,
	 20,  23,  20,  103, 8,   127, 8,   127, 89,  244, 18,  78,  30, 2,
	 134, 97,  16,  119, 48,  79,  44,  46,  125, 30,  0,   102, 36, 55,
	 60,  63,  19,  0,   69,  164, 190, 22,  60,  62,  148, 182, 36, 119,
	 48,  255, 146, 119, 20,  55,  0,   110, 36,  46,  20,  22,  52, 22,
	 8,   127, 0,   38,  150, 22,  16,  22,  8,   6,   126, 95,  8,  38,
	 20,  181, 121, 30,  16,  103, 72,  127, 16,  78,  20,  4,   88, 247,
	 32,  79,  8,   95,  84,  149, 117, 165, 89,  148, 56,  95,  60, 79,
	 36,  46,  20,  132, 144, 245, 16,  22,  16,  101, 125, 182, 93, 6,
	 16,  100, 0,   109, 16,  76,  0,   78,  88,  87,  105, 127, 32, 76,
	 32,  111, 8,   14,  180, 237, 18,  22},

	{8,   18,  146, 22,  146, 54,  186, 26,  4,   39,  72,  118, 16,  68,
	 26,  22,  4,   54,  24,  62,  28,  6,   136, 34,  12,  38,  69,  164,
	 48,  127, 24,  22,  36,  165, 48,  77,  5,   164, 5,   4,   8,   94,
	 140, 54,  100, 181, 4,   4,   146, 68,  136, 123, 8,   127, 140, 34,
	 4,   119, 16,  6,   130, 34,  144, 245, 58,  10,  65,  224, 1,   229,
	 24,  22,  0,   180, 52,  111, 8,   78,  104, 94,  146, 50,  88,  94,
	 16,  110, 24,  22,  44,  127, 105, 44,  14,  18,  16,  86,  73,  76,
	 44,  95,  52,  127, 73,  76,  73,  104, 44,  91,  24,  22,  37,  205,
	 9,   76,  146, 95,  26,  22,  12,  22,  18,  79,  154, 74,  36,  183,
	 20,  215, 154, 2,   16,  118, 16,  86,  72,  54,  24,  22,  16,  102,
	 24,  86,  18,  70,  22,  6,   44,  127, 56,  95,  73,  237, 73,  236,
	 48,  94,  0,   78,  81,  204, 8,   38,  12,  54,  0,   36},

	{104, 30,  72,  126, 20,  55,  8,   78,  76,  54,  26,  22,  4,   180,
	 12,  22,  4,   55,  140, 54,  69,  132, 73,  246, 73,  182, 20,  22,
	 73,  246, 80,  214, 16,  22,  109, 158, 0,   100, 24,  22,  44,  30,
	 92,  247, 0,   104, 87,  149, 44,  31,  52,  87,  60,  31,  124, 247,
	 152, 103, 117, 253, 36,  55,  52,  87,  203, 115, 44,  127, 36,  55,
	 21,  36,  209, 237, 52,  229, 0,   110, 0,   119, 158, 18,  52,  111,
	 8,   86,  16,  103, 24,  86,  237, 182, 4,   160, 52,  7,   16,  108,
	 200, 182, 217, 180, 20,  54,  4,   164, 88,  126, 0,   193, 73,  247,
	 36,  237, 121, 94,  20,  22,  8,   38,  16,  54,  24,  6,   26,  22,
	 16,  22,  28,  22,  4,   38,  28,  22,  20,  182, 52,  31,  85,  181,
	 150, 180, 32,  46,  20,  55,  0,   111, 16,  70,  0,   103, 4,   38,
	 0,   102, 24,  22,  144, 102, 121, 30,  16,  54,  20,  6,   8,   22,
	 28,  22,  8,   22,  80,  182, 40,  46,  20,  87,  24,  22,  4,   103,
	 8,   14},

	{144, 38,  72,  22,  88,  94,  4,   39,  72,  94,  4,  180, 8,   102,
	 8,   70,  0,   118, 12,  54,  16,  70,  0,   6,   52, 127, 75,  2,
	 36,  127, 8,   22,  39,  73,  44,  95,  40,  111, 0,  100, 19,  4,
	 154, 114, 146, 64,  52,  127, 52,  247, 146, 247, 25, 6,   17,  77,
	 4,   132, 109, 127, 73,  62,  105, 110, 73,  38,  56, 111, 16,  70,
	 72,  118, 0,   22,  154, 87,  73,  72,  36,  111, 8,  103, 30,  22,
	 52,  95,  8,   110, 0,   79,  105, 79,  48,  12,  85, 180, 148, 38,
	 20,  38,  16,  54,  8,   22,  0,   6,   24,  102, 9,  38,  0,   118,
	 128, 228, 65,  244, 44,  111, 0,   119, 40,  111, 24, 86,  8,   79,
	 73,  255, 8,   79,  24,  79,  154, 66,  154, 22,  24, 87,  105, 255,
	 16,  86,  132, 38,  54,  22,  0,   36,  0,   119, 91, 22,  73,  150,
	 73,  79,  8,   127, 18,  196, 18,  68,  8,   6,   16, 70,  73,  38,
	 52,  69,  16,  87,  26,  86,  73,  245, 16,  71,  18, 66,  4,   54,
	 8,   38,  8,   54,  93,  22},

	{8,   30,  20, 54,  40,  94,  148, 39,  20,  36,  73,  182, 48,  110,
	 93,  182, 8,  54,  20,  6,   40,  95,  24,  86,  85,  181, 44,  95,
	 40,  62,  8,  22,  8,   22,  0,   36,  8,   22,  81,  132, 22,  23,
	 16,  102, 16, 70,  16,  196, 244, 255, 16,  205, 83,  212, 60,  31,
	 32,  127, 40, 111, 73,  108, 44,  31,  50,  205, 255, 193, 132, 225,
	 24,  6,   26, 22,  105, 14,  20,  37,  254, 63,  210, 217, 16,  22,
	 61,  7,   89, 87,  4,   38,  127, 79,  8,   38,  88,  118, 27,  78,
	 9,   6,   89, 22,  76,  182, 105, 30,  16,  100, 201, 50,  9,   78,
	 20,  38,  16, 247, 144, 182, 89,  76,  44,  63,  24,  118, 150, 54,
	 36,  119, 4,  6,   16,  36,  8,   22,  0,   38,  20,  36,  0,   102,
	 8,   22,  88, 150, 0,   38,  24,  22,  90,  18,  72,  86,  26,  18,
	 16,  4,   0,  103, 56,  127, 240, 255, 34,  72,  20,  197, 109, 111,
	 16,  204, 69, 132, 205, 182, 121, 255, 1,   100, 88,  246, 24,  22,
	 20,  197, 18, 132, 8,   22,  17,  68,  90,  30,  73,  38,  17,  76,
	 124, 151, 8,  102, 24,  22,  8,   118, 0,   118, 0,   100, 18,  4,
	 52,  79,  48, 205, 65,  196, 16,  70,  125, 181, 65,  132},

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
