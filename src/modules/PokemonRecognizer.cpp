#include "PokemonRecognizer.h"

static constexpr int NORMALIZED_HEIGHT = 98;
static constexpr int DESCRIPTOR_SIZE = 16;

std::string PokemonRecognizer::recognizePokemon(const cv::Mat &imageBGR,
						const cv::Mat &mask) const
{
	cv::Mat scaledBGR;
	cv::resize(imageBGR, scaledBGR,
		   cv::Size(imageBGR.cols * NORMALIZED_HEIGHT / imageBGR.rows,
			    NORMALIZED_HEIGHT));

	std::vector<cv::KeyPoint> targetKeyPoints;
	cv::Mat targetDescriptors;
	auto algorithm =
		cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, DESCRIPTOR_SIZE);
	algorithm->detectAndCompute(scaledBGR, mask, targetKeyPoints,
				    targetDescriptors);

	auto matcher = cv::DescriptorMatcher::create("BruteForce");
	std::vector<double> results;
	for (int i = 0; i < static_cast<int>(POKEMON_DESCRIPTORS.size()); i++) {
		std::vector<cv::DMatch> matches;
		const cv::Mat candidateDescriptor =
			POKEMON_DESCRIPTORS[i % POKEMON_DESCRIPTORS.size()];
		matcher->match(targetDescriptors, candidateDescriptor, matches);

		double sum = 0.0;
		for (auto match : matches) {
			sum += match.distance;
		}

		results.push_back(sum / (double)matches.size());
	}

	const auto index =
		std::distance(results.begin(),
			      std::min_element(results.begin(), results.end()));

	return POKEMON_IDS[index];
}
