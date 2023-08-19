#pragma once

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

class MySelectionRecognizer {
public:
	MySelectionRecognizer(int _blueThreshold, int _binaryThreshould,
			    double _ratio, const std::vector<int> &_indices,
			    const std::vector<int> &_cols,
			    const std::vector<std::vector<uchar>> &_data)
		: blueThreshold(_blueThreshold),
		  binaryThreshold(_binaryThreshould),
		  ratio(_ratio),
		  indices(_indices),
		  cols(_cols),
		  data(_data),
		  templates(generateMatVector(data, cols, CV_8U))
	{
	}

	int operator()(const cv::Mat &imageBGR, const cv::Mat &imageGray) const
	{
		if (imageBGR.at<cv::Vec3b>(0, 0)[0] < blueThreshold)
			return 0;

		cv::Mat imageBinary;
		cv::threshold(imageGray, imageBinary, binaryThreshold, 255,
			      cv::THRESH_BINARY);
		int minIndex = -1;
		double minValue = static_cast<double>(imageBinary.total());
		for (size_t i = 0; i < templates.size(); i++) {
			const cv::Mat &templ = templates[i];
			const cv::Mat resultImage = imageBinary ^ templ;
			const double difference =
				cv::sum(resultImage)[0] / 255.0;
			if (difference < minValue) {
				minIndex = static_cast<int>(i);
				minValue = difference;
			}
		}
		if (minValue <
		    static_cast<double>(imageBinary.total()) * ratio) {
			return indices[minIndex];
		} else {
			return 0;
		}
	}

private:
	const int blueThreshold;
	const int binaryThreshold;
	const double ratio;
	const std::vector<int> indices;
	const std::vector<int> cols;
	const std::vector<std::vector<uchar>> data;
	const std::vector<cv::Mat> templates;

	static const std::vector<cv::Mat>
	generateMatVector(const std::vector<std::vector<uchar>> &data,
			  const std::vector<int> &cols, int type)
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
