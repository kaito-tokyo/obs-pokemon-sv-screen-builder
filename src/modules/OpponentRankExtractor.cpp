#include "OpponentRankExtractor.h"
#include <obs.h>

const std::string LANGUAGE = "ja";

int matchParenStart(const cv::Mat &lineBinary, const cv::Mat &parenStart)
{
	for (int i = lineBinary.cols - parenStart.cols; i >= 0; i--) {
		cv::Rect matchingRect(i, 0, parenStart.cols, parenStart.rows);
		cv::Mat matchResult = parenStart ^ lineBinary(matchingRect);
		const double difference = cv::sum(matchResult)[0] / 255.0;
		if (difference < parenStart.total() * 0.05) {
			return i + parenStart.cols;
		}
	}
	return -1;
}

int matchParenEnd(const cv::Mat &lineBinary, const cv::Mat &parenEnd,
		  int rankStart)
{
	for (int i = rankStart; i < lineBinary.cols - parenEnd.cols; i++) {
		cv::Rect matchingRect(i, 0, parenEnd.cols, parenEnd.rows);
		cv::Mat matchResult = parenEnd ^ lineBinary(matchingRect);
		const double difference = cv::sum(matchResult)[0] / 255.0;
		if (difference < parenEnd.total() * 0.05) {
			return i;
		}
	}
	return -1;
}

cv::Rect OpponentRankExtractor::extract(const cv::Mat &screenBinary) const
{
	cv::Mat lineBinary = screenBinary(lineRect);
	const auto parenIndices = PAREN_MAP.at(language);

	cv::Mat parenStart = PAREN_TEMPLATES[parenIndices.first];
	const int rankStart = matchParenStart(lineBinary, parenStart);
	if (rankStart < 0)
		return cv::Rect();

	cv::Mat parenEnd = PAREN_TEMPLATES[parenIndices.second];
	const int rankEnd = matchParenEnd(lineBinary, parenEnd, rankStart);
	if (rankEnd < 0)
		return cv::Rect();

	const int width = rankEnd - rankStart;
	return cv::Rect(lineRect.x + rankStart, lineRect.y, width,
			lineRect.height);
}
