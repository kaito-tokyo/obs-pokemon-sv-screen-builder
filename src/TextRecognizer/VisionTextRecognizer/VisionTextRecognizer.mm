#include <functional>
#include <string>

#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import <Vision/Vision.h>

#include <opencv2/opencv.hpp>

#include <obs.h>

#include "plugin-support.h"

cv::Mat preprocessCvMat(const cv::Mat &src)
{
	cv::Mat resized;
	cv::resize(src, resized, cv::Size(src.cols * 4, src.rows * 4));

	cv::Mat padded;
	cv::copyMakeBorder(resized, padded, resized.rows / 4, resized.rows / 4,
			   resized.cols / 4, resized.cols / 4,
			   cv::BORDER_CONSTANT, cv::Scalar(255));

	return padded;
}

CGImageRef convertCvMatToCGImage(const cv::Mat &cvMat, OSType pixelFormatType)
{
	CVPixelBufferRef pixelBuffer;
	CVReturn retPixelBuffer = CVPixelBufferCreateWithBytes(
		NULL, cvMat.cols, cvMat.rows, pixelFormatType, cvMat.data,
		cvMat.cols, NULL, NULL, NULL, &pixelBuffer);
	if (retPixelBuffer != kCVReturnSuccess) {
		obs_log(LOG_ERROR, "CVPixelBuffer creation failed! %d",
			retPixelBuffer);
		if (pixelBuffer != NULL) {
			CVPixelBufferRelease(pixelBuffer);
		}
		return NULL;
	}

	CGImageRef image;
	OSStatus retImage =
		VTCreateCGImageFromCVPixelBuffer(pixelBuffer, NULL, &image);
	if (retImage != noErr) {
		obs_log(LOG_ERROR, "CGImage creation failed!");
		if (image != NULL) {
			CGImageRelease(image);
		}
		return NULL;
	}
	CVPixelBufferRelease(pixelBuffer);

	return image;
}

std::string recognizeByVision(CGImageRef image, NSString *langCode)
{
	__block std::string resultText;

	VNImageRequestHandler *requestHandler = [[VNImageRequestHandler alloc]
		initWithCGImage:image
			options:@{}];

	VNRequestCompletionHandler completionHandler = ^(VNRequest *req,
							 NSError *err) {
		if (err) {
			NSLog(@"%@", err);
			return;
		}
		for (VNRecognizedTextObservation *observation in req.results) {
			NSArray<VNRecognizedText *> *candidates =
				[observation topCandidates:1];
			VNRecognizedText *recognizedText =
				[candidates firstObject];
			NSString *nsString = recognizedText.string;
			resultText += [nsString UTF8String];
		}
	};

	VNRecognizeTextRequest *request = [[VNRecognizeTextRequest alloc]
		initWithCompletionHandler:completionHandler];
	request.minimumTextHeight = 0.8;
	request.recognitionLevel = VNRequestTextRecognitionLevelAccurate;
	request.recognitionLanguages = @[langCode];

	NSError *_Nullable error;
	[requestHandler performRequests:@[request] error:&error];

	return resultText;
}

void recognizeText(const cv::Mat imageBinary,
		   std::function<void(std::string)> callback)
{
	dispatch_queue_t queue =
		dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

	dispatch_block_t block = ^{
		cv::Mat padded = preprocessCvMat(imageBinary);
		CGImageRef image = convertCvMatToCGImage(
			padded, kCVPixelFormatType_OneComponent8);
		std::string result = recognizeByVision(image, @"ja-JP");
		CGImageRelease(image);
		callback(result);
	};

	dispatch_async(queue, block);
}
