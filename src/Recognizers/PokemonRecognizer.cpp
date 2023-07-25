#include "PokemonRecognizer.h"

static void extractAFromBGRA(const cv::Mat &imageBGRA, cv::Mat &imageA)
{
	imageA = cv::Mat(imageBGRA.rows, imageBGRA.cols, CV_8U);
	for (int y = 0; y < imageBGRA.rows; y++) {
		for (int x = 0; x < imageBGRA.cols; x++) {
			imageA.at<uchar>(y, x) =
				imageBGRA.at<cv::Vec4b>(y, x)[3];
		}
	}
}

std::string PokemonRecognizer::recognizePokemon(const cv::Mat &imageBGRA) const
{
	cv::Mat scaledBGRA, scaledBGR, mask;
	cv::resize(imageBGRA, scaledBGRA,
		   cv::Size(imageBGRA.cols * HEIGHT / imageBGRA.rows, HEIGHT));
	cv::cvtColor(scaledBGRA, scaledBGR, cv::COLOR_BGRA2BGR);
	extractAFromBGRA(scaledBGRA, mask);

	std::vector<cv::KeyPoint> targetKeyPoints;
	cv::Mat targetDescriptors;
	auto algorithm =
		cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, DESCRIPTOR_SIZE);
	algorithm->detectAndCompute(scaledBGR, mask, targetKeyPoints,
				    targetDescriptors);

	auto matcher = cv::DescriptorMatcher::create("BruteForce");
	std::vector<double> results;
	for (int i = 0; i < static_cast<int>(DESCRIPTORS.size()); i++) {
		std::vector<cv::DMatch> matches;
		const cv::Mat candidateDescriptor = DESCRIPTORS[i];
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

	return POKEMON_NAMES[index];
}
