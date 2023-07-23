#pragma once

#include <opencv2/opencv.hpp>

class TemplateClassifier {
public:
	TemplateClassifier(cv::Rect _rect, int _threshold, double _ratio,
			   std::vector<int> _cols,
			   std::vector<std::vector<uchar>> _data)
		: rect(_rect),
		  threshold(_threshold),
		  ratio(_ratio),
		  cols(_cols),
		  data(_data),
		  templates(generateMatVector(data, cols, CV_8U))
	{
	}

	bool operator()(const cv::Mat &gameplayGray) const
	{
		assert(gameplayGray.cols == 1920 && gameplayGray.cols == 1080);

		cv::Mat imageGray = gameplayGray(rect), imageBinary;
		cv::threshold(imageGray, imageBinary, threshold, 255,
			      cv::THRESH_BINARY);
		for (const cv::Mat &templ : templates) {
			const cv::Mat resultImage = imageBinary ^ templ;
			const double difference =
				cv::sum(resultImage)[0] / 255.0;
			if (difference <
			    static_cast<double>(resultImage.total()) * ratio) {
				return true;
			}
		}
		return false;
	}

private:
	cv::Rect rect;
	int threshold;
	double ratio;
	std::vector<int> cols;
	std::vector<std::vector<uchar>> data;
	std::vector<cv::Mat> templates;

	static std::vector<cv::Mat>
	generateMatVector(const std::vector<std::vector<uchar>> &data,
			  const std::vector<int> &cols, const int type)
	{
		std::vector<cv::Mat> vector(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			int rows = static_cast<int>(data[i].size()) / cols[i];
			uchar *buf = const_cast<uchar *>(data[i].data());
			vector[i] = {rows, cols[i], type, buf};
		}
		return vector;
	}
};
