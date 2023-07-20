#include <obs.h>

#include "MyRankExtractor.h"

int matchIcon(const cv::Mat &lineBinary, const cv::Mat &icon)
{
	for (int i = 0; i < lineBinary.cols - icon.cols; i++) {
		cv::Rect matchingRect(i, 0, icon.cols, icon.rows);
		cv::Mat matchResult = icon ^ lineBinary(matchingRect);
		const double difference = cv::sum(matchResult)[0] / 255.0;
		if (difference < static_cast<double>(icon.total()) * 0.2) {
			return i + icon.cols;
		}
	}
	return -1;
}

cv::Rect MyRankExtractor::extract(const cv::Mat &gameplayBinary) const
{
	cv::Mat lineBinary = gameplayBinary(lineRect);
	cv::Mat icon = ICON_TEMPLATES[0];
	const int rankStart = matchIcon(lineBinary, icon);
	if (rankStart < 0) {
		return cv::Rect();
	} else {
		return cv::Rect(lineRect.x + rankStart, lineRect.y,
				lineBinary.cols - rankStart, lineRect.height);
	}
}
