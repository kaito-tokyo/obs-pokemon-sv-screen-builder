#include "OpponentRankExtractor.h"
#include <obs.h>

void OpponentRankExtractor::extract(const cv::Mat &screenTextBinary)
{
	cv::Range colRange{544, 1378}, rowRange{831, 867};
	cv::Mat lineBinary = screenTextBinary(rowRange, colRange);
	const int parenStartIndex = PAREN_MAP.at(std::string("ja")).first;
	const int parenEndIndex = PAREN_MAP.at(std::string("ja")).second;
	cv::Mat parenStart = PAREN_TEMPLATES[parenStartIndex];
	cv::Mat parenEnd = PAREN_TEMPLATES[parenEndIndex];
	for (int x = lineBinary.cols - parenStart.cols; x >= 0; x--) {
		cv::Range lineColRange{x, x + parenStart.cols},
			lineRowRange{0, parenStart.rows};
		cv::Mat resultImage = lineBinary(lineColRange, lineRowRange) ^
				      parenStart;
		const cv::Scalar difference = cv::sum(resultImage) / 255;
		blog(LOG_INFO, "start %f", difference[0]);
	}
}
