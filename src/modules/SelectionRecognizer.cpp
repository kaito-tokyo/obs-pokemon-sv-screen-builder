#include "SelectionRecognizer.h"

static constexpr int BLUE_THRESHOLD = 200;

int SelectionRecognizer::recognizeSelection(const cv::Mat &imageBGR) const
{
	if (imageBGR.at<cv::Vec3b>(0, 0)[0] < BLUE_THRESHOLD)
		return 0;

	cv::Mat image;
	cv::cvtColor(imageBGR, image, cv::COLOR_BGR2GRAY);
	cv::threshold(image, image, BINARY_THRESHOLD, 255, cv::THRESH_BINARY);

	std::vector<double> results;
	for (size_t i = 0; i < SELECTION_TEMPLATES.size(); i++) {
		const cv::Mat &resultImage = image & SELECTION_TEMPLATES[i];
		const auto target = cv::sum(SELECTION_TEMPLATES[i] / 255);
		const auto actual = cv::sum(resultImage / 255);
		if (cv::abs(actual[0] - target[0]) < target[0] * 0.2) {
			return SELECTION_INDEX[i];
		}
	}

	return 0;
}
