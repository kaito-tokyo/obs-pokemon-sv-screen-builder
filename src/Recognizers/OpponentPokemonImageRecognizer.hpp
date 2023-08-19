#pragma once

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

class OpponentPokemonImageRecognizer {
public:
	OpponentPokemonImageRecognizer(int _height, int _descriptorSize,
			  std::vector<std::vector<uchar>> _data,
			  std::vector<std::string> _pokemonNames)
		: height(_height),
		  descriptorSize(_descriptorSize),
		  data(_data),
		  descriptors(
			  generateDescriptors((descriptorSize + 7) / 8, data)),
		  pokemonNames(_pokemonNames)
	{
	}

	std::string operator()(const cv::Mat &imageBGRA) const
	{
		cv::Mat scaledBGRA, scaledBGR;
		cv::resize(imageBGRA, scaledBGRA,
			   cv::Size(imageBGRA.cols * height / imageBGRA.rows,
				    height));
		cv::cvtColor(scaledBGRA, scaledBGR, cv::COLOR_BGRA2BGR);
		cv::Mat mask = extractAFromBGRA(scaledBGRA);

		std::vector<cv::KeyPoint> targetKeyPoints;
		cv::Mat targetDescriptors;
		auto algorithm = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB,
						   descriptorSize);
		algorithm->detectAndCompute(scaledBGR, mask, targetKeyPoints,
					    targetDescriptors);

		auto matcher = cv::DescriptorMatcher::create("BruteForce");
		std::vector<double> results;
		for (int i = 0; i < static_cast<int>(descriptors.size()); i++) {
			std::vector<cv::DMatch> matches;
			const cv::Mat candidateDescriptor = descriptors[i];
			matcher->match(targetDescriptors, candidateDescriptor,
				       matches);

			double sum = 0.0;
			for (auto match : matches) {
				sum += match.distance;
			}

			results.push_back(sum / (double)matches.size());
		}

		const auto index = std::distance(
			results.begin(),
			std::min_element(results.begin(), results.end()));

		return pokemonNames[index];
	}

private:
	const int height;
	const int descriptorSize;
	const std::vector<std::vector<uchar>> data;
	const std::vector<cv::Mat> descriptors;
	const std::vector<std::string> pokemonNames;

	static const std::vector<cv::Mat>
	generateDescriptors(int cols,
			    const std::vector<std::vector<uchar>> &data)
	{
		std::vector<cv::Mat> descriptors(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			descriptors[i] = cv::Mat(
				static_cast<int>(data[i].size()) / cols, cols,
				CV_8U, const_cast<uchar *>(data[i].data()));
		}
		return descriptors;
	}

	cv::Mat extractAFromBGRA(const cv::Mat &imageBGRA) const
	{
		cv::Mat imageA(imageBGRA.rows, imageBGRA.cols, CV_8U);
		for (int y = 0; y < imageBGRA.rows; y++) {
			for (int x = 0; x < imageBGRA.cols; x++) {
				imageA.at<uchar>(y, x) =
					imageBGRA.at<cv::Vec4b>(y, x)[3];
			}
		}
		return imageA;
	}
};
