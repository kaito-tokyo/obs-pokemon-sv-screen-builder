#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class OpponentPokemonCropper {
public:
	OpponentPokemonCropper(std::vector<cv::Rect> _rects, cv::Point _seed,
			       cv::Scalar _loDiff, cv::Scalar _upDiff)
		: rects(_rects),
		  seed(_seed),
		  loDiff(_loDiff),
		  upDiff(_upDiff)
	{
	}

	std::vector<cv::Mat> crop(const cv::Mat &gameplay) const
	{
		std::vector<cv::Mat> output(rects.size());
		for (size_t i = 0; i < rects.size(); i++) {
			output[i] = gameplay(rects[i]);
		}
		return output;
	}

	std::vector<cv::Mat>
	generateMask(const std::vector<cv::Mat> &imagesBGR) const
	{
		std::vector<cv::Mat> masks(imagesBGR.size());
		for (size_t i = 0; i < imagesBGR.size(); i++) {
			cv::Mat image = imagesBGR[i], mask;
			cv::floodFill(image, mask, seed, 0, nullptr, loDiff,
				      upDiff, cv::FLOODFILL_MASK_ONLY);

			const int borderWidth = 3;
			mask(cv::Rect(0, 0, mask.cols, borderWidth)).setTo(cv::Scalar(0));
			mask(cv::Rect(0, mask.rows - borderWidth, mask.cols, borderWidth)).setTo(cv::Scalar(0));
			mask(cv::Rect(0, 0, borderWidth, mask.rows)).setTo(cv::Scalar(0));
			mask(cv::Rect(mask.cols - borderWidth, 0, borderWidth, mask.rows)).setTo(cv::Scalar(0));

			masks[i] = mask({1, 1, mask.cols - 2, mask.rows - 2});
		}
		return masks;
	}

	std::vector<cv::Mat>
	generateTransparentImages(const std::vector<cv::Mat> &inputsBGRA,
				  const std::vector<cv::Mat> &masks) const
	{
		assert(inputsBGRA.size() == masks.size());

		std::vector<cv::Mat> outputsBGRA(inputsBGRA.size());
		for (size_t i = 0; i < inputsBGRA.size(); i++) {
			outputsBGRA[i] = inputsBGRA[i].clone();
			copyMaskToBGRA(outputsBGRA[i], masks[i]);
		}
		return outputsBGRA;
	}

private:
	const std::vector<cv::Rect> rects;
	const cv::Point seed;
	const cv::Scalar loDiff;
	const cv::Scalar upDiff;

	void copyMaskToBGRA(cv::Mat &imageBGRA, const cv::Mat &mask) const
	{
		assert(imageBGRA.rows == mask.rows &&
		       imageBGRA.cols == mask.cols);

		for (int y = 0; y < imageBGRA.rows; y++) {
			for (int x = 0; x < imageBGRA.cols; x++) {
				cv::Vec4b &dest = imageBGRA.at<cv::Vec4b>(y, x);
				uchar src = mask.at<uchar>(y, x);
				dest[3] = static_cast<uchar>((1 - src) * 255);
			}
		}
	}
};
