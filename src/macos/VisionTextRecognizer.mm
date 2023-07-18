#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "VisionTextRecognizer.h"

std::string recognizeText(const cv::Mat& imageBGRA)
{
    CVPixelBufferRef pixelBuffer;
    int width = imageBGRA.cols;
    int height = imageBGRA.rows;
    CVPixelBufferCreateWithBytes(kCFAllocatorDefault, width, height, kCVPixelFormatType_32BGRA, imageBGRA.data, width * 4, nil, nil, nil, &pixelBuffer);
    return std::string("aaa") + std::to_string(imageBGRA.rows);
}
