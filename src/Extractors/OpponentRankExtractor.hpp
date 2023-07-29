#pragma once

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

class OpponentRankExtractor {
public:
	OpponentRankExtractor(
		cv::Rect _rect, int _threshold,
		const std::map<std::string, std::pair<int, int>> &_parenMap,
		const std::vector<int> _cols,
		const std::vector<std::vector<uchar>> &_data, double _ratio)
		: rect(_rect),
		  threshold(_threshold),
		  parenMap(_parenMap),
		  cols(_cols),
		  data(_data),
		  templates(generateMatVector(cols, data, CV_8U)),
		  ratio(_ratio)
	{
	}

	cv::Rect operator()(const cv::Mat &gameplayGray) const
	{
		assert(gameplayGray.cols == 1920 && gameplayGray.rows == 1080);

		cv::Mat lineGray = gameplayGray(rect), lineBinary;
		cv::threshold(lineGray, lineBinary, threshold, 255,
			      cv::THRESH_BINARY);
		const auto parenIndices = parenMap.at("ja");

		cv::Mat parenStart = templates[parenIndices.first];
		const int rankStart = matchParenStart(lineBinary, parenStart);
		if (rankStart < 0) {
			return {};
		}

		cv::Mat parenEnd = templates[parenIndices.second];
		const int rankEnd =
			matchParenEnd(lineBinary, parenEnd, rankStart);
		if (rankEnd < 0) {
			return {};
		}

		const int width = rankEnd - rankStart;
		return {rect.x + rankStart, rect.y, width, rect.height};
	}

private:
	const cv::Rect rect;
	const int threshold;
	const std::map<std::string, std::pair<int, int>> parenMap;
	const std::vector<int> cols;
	const std::vector<std::vector<uchar>> data;
	const std::vector<cv::Mat> templates;
	double ratio;

	static std::vector<cv::Mat>
	generateMatVector(const std::vector<int> &cols,
			  const std::vector<std::vector<uchar>> &data, int type)
	{
		assert(cols.size() == data.size());
		std::vector<cv::Mat> vector(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			const int rows =
				static_cast<int>(data[i].size()) / cols[i];
			uchar *buf = const_cast<uchar *>(data[i].data());
			vector[i] = {rows, cols[i], type, buf};
		}
		return vector;
	}

	int matchParenStart(const cv::Mat &lineBinary,
			    const cv::Mat &parenStart) const
	{
		for (int i = lineBinary.cols - parenStart.cols; i >= 0; i--) {
			cv::Rect matchingRect(i, 0, parenStart.cols,
					      parenStart.rows);
			cv::Mat matchResult = parenStart ^
					      lineBinary(matchingRect);
			const double difference =
				cv::sum(matchResult)[0] / 255.0;
			if (difference <
			    static_cast<double>(parenStart.total()) * ratio) {
				return i + parenStart.cols;
			}
		}
		return -1;
	}

	int matchParenEnd(const cv::Mat &lineBinary, const cv::Mat &parenEnd,
			  int rankStart) const
	{
		for (int i = rankStart; i < lineBinary.cols - parenEnd.cols;
		     i++) {
			cv::Rect matchingRect(i, 0, parenEnd.cols,
					      parenEnd.rows);
			cv::Mat matchResult = parenEnd ^
					      lineBinary(matchingRect);
			const double difference =
				cv::sum(matchResult)[0] / 255.0;
			if (difference <
			    static_cast<double>(parenEnd.total()) * ratio) {
				return i;
			}
		}
		return -1;
	}
};
