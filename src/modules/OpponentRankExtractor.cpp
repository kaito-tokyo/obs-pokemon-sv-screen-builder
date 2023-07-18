#include "OpponentRankExtractor.h"
#include <obs.h>

void OpponentRankExtractor::extract(const cv::Mat &screenTextBinary,
				    const cv::Mat &screenBGRA)
{
	cv::Range colRange{544, 1378}, rowRange{831, 867};
	cv::Mat lineBinary = screenTextBinary(rowRange, colRange);
	const int parenStartIndex = PAREN_MAP.at(std::string("ja")).first;
	const int parenEndIndex = PAREN_MAP.at(std::string("ja")).second;
	cv::Mat parenStart = PAREN_TEMPLATES[parenStartIndex];
	cv::Mat parenEnd = PAREN_TEMPLATES[parenEndIndex];
	for (int x0 = lineBinary.cols - parenStart.cols; x0 >= 0; x0--) {
		cv::Range lineColRange0{x0, x0 + parenStart.cols},
			lineRowRange0{0, parenStart.rows};
		cv::Mat resultImage0 =
			lineBinary(lineRowRange0, lineColRange0) ^ parenStart;
		const cv::Scalar difference0 = cv::sum(resultImage0) / 255;
		if (difference0[0] < 5) {
			int rankStart = x0 + parenStart.cols;
			for (int x1 = rankStart;
			     x1 < lineBinary.cols - parenEnd.cols; x1++) {
				cv::Range lineColRange1{x1, x1 + parenEnd.cols},
					lineRowRange1{0, parenEnd.rows};
				cv::Mat resultImage1 =
					lineBinary(lineRowRange1,
						   lineColRange1) ^
					parenEnd;
				const cv::Scalar difference1 =
					cv::sum(resultImage1) / 255;
				if (difference1[0] < 5) {
					int rankEnd = x1;
					cv::Mat lineBGRA =
						screenBGRA(rowRange, colRange);
					imageBGRA = lineBGRA.colRange(rankStart,
								      rankEnd)
							    .clone();
				}
			}
		}
	}
}
