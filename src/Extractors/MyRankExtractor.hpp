#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class MyRankExtractor {
public:
	MyRankExtractor(cv::Rect _rect, int _threshold,
			const std::vector<int> _cols,
			const std::vector<std::vector<uchar>> &_data,
			double _ratio)
		: rect(_rect),
		  threshold(_threshold),
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
		cv::Mat icon = templates[0];
		const int rankStart = matchIcon(lineBinary, icon);
		if (rankStart < 0) {
			return {};
		} else {
			return {rect.x + rankStart, rect.y,
				lineBinary.cols - rankStart, rect.height};
		}
	}

private:
	const cv::Rect rect;
	const int threshold;
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

	int matchIcon(const cv::Mat &lineBinary, const cv::Mat &icon) const
	{
		for (int i = 0; i < lineBinary.cols - icon.cols; i++) {
			cv::Rect matchingRect(i, 0, icon.cols, icon.rows);
			cv::Mat matchResult = icon ^ lineBinary(matchingRect);
			const double difference =
				cv::sum(matchResult)[0] / 255.0;
			if (difference <
			    static_cast<double>(icon.total()) * ratio) {
				return i + icon.cols;
			}
		}
		return -1;
	}
};
