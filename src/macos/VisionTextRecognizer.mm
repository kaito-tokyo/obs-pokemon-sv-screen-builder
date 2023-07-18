#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import <Vision/Vision.h>
#import "VisionTextRecognizer.h"

#include <obs.h>

CGImageRef convertBGRAtoCgImage(const cv::Mat &imageBGRA)
{
	CVPixelBufferRef pixelBuffer;
	CVReturn retPixelBuffer = CVPixelBufferCreateWithBytes(
		kCFAllocatorDefault, imageBGRA.cols, imageBGRA.rows,
		kCVPixelFormatType_32BGRA, imageBGRA.data, imageBGRA.cols * 4,
		NULL, NULL, NULL, &pixelBuffer);
	if (retPixelBuffer != kCVReturnSuccess) {
		blog(LOG_ERROR, "CVPixelBuffer creation failed!");
		if (pixelBuffer != NULL) {
			CFRelease(pixelBuffer);
		}
		return NULL;
	}

	CGImageRef image;
	OSStatus retImage =
		VTCreateCGImageFromCVPixelBuffer(pixelBuffer, NULL, &image);
	if (retImage != noErr) {
		blog(LOG_ERROR, "CGImage creation failed!");
		if (image != NULL) {
			CFRelease(image);
		}
		return NULL;
	}
	CFRelease(pixelBuffer);

	return image;
}

class VisionTextRecognizer {
public:
	std::string recognizeByVision(CGImageRef image);

private:
	std::string resultText;
};

std::string VisionTextRecognizer::recognizeByVision(CGImageRef image)
{
	VNImageRequestHandler *requestHandler = [[VNImageRequestHandler alloc]
		initWithCGImage:image
			options:@{}];
	VNRecognizeTextRequest *request = [[VNRecognizeTextRequest alloc]
		initWithCompletionHandler:^(VNRequest *req, NSError *err) {
			if (err) {
				NSLog(@"%@", err);
				return;
			}
			for (VNRecognizedTextObservation *observation in req
				     .results) {
				NSArray<VNRecognizedText *> *candidates =
					[observation topCandidates:1];
				VNRecognizedText *recognizedText =
					[candidates firstObject];
				NSString *nsString = recognizedText.string;
				resultText += [nsString UTF8String];
			}
		}];
	request.recognitionLanguages = @[@"ja"];
	NSError *_Nullable error;
	[requestHandler performRequests:@[request] error:&error];
	return resultText;
}

std::string recognizeText(const cv::Mat &imageBGRA)
{
	CGImageRef image = convertBGRAtoCgImage(imageBGRA);
	if (image == NULL)
		return std::string();
	VisionTextRecognizer recognizer;
	std::string result = recognizer.recognizeByVision(image);
	CFRelease(image);
	return result;
}
