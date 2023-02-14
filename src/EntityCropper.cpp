#include <opencv2/opencv.hpp>

#include "EntityCropper.h"

void EntityCropper::crop(const cv::Mat &screenBGRA) {
  double xScale = screenBGRA.cols / 1920.0, yScale = screenBGRA.rows / 1080.0;
  for (size_t i = 0; i < rangesRow.size(); i++) {
    const cv::Range colRange(rangeCol[0] * xScale, rangeCol[1] * xScale),
        rowRange(rangesRow[i][0] * yScale, rangesRow[i][1] * yScale);
    imagesBGRA[i] = screenBGRA(rowRange, colRange);
    cv::cvtColor(imagesBGRA[i], imagesBGR[i], cv::COLOR_BGRA2BGR);
  }
}

void EntityCropper::generateMask(void) {
  for (size_t i = 0; i < rangesRow.size(); i++) {
    cv::Mat mask;
    cv::Scalar diff(32, 32, 32);
    cv::floodFill(imagesBGR[i], mask, seedPoint, 0, 0, diff, diff,
                  cv::FLOODFILL_MASK_ONLY);
    cv::Range maskCropCol(1, mask.cols - 1), maskCropRow(1, mask.rows - 1);
    masks[i] = 1 - mask(maskCropRow, maskCropCol);

    for (int y = 0; y < imagesBGRA[i].rows; y++) {
      for (int x = 0; x < imagesBGRA[i].cols; x++) {
        imagesBGRA[i].at<cv::Vec4b>(y, x)[3] = masks[i].at<uchar>(y, x) * 255;
      }
    }
  }
}