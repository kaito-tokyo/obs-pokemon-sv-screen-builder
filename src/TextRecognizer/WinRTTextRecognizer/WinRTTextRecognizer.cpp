#include <functional>
#include <string>

#include <opencv2/opencv.hpp>

#include <unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Ocr.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Globalization.h>
#include <MemoryBuffer.h>

using winrt::Windows::Media::Ocr::OcrEngine;
using winrt::Windows::Media::Ocr::OcrResult;
using winrt::Windows::Globalization::Language;
using winrt::Windows::Graphics::Imaging::SoftwareBitmap;
using winrt::Windows::Graphics::Imaging::BitmapPixelFormat;
using winrt::Windows::Storage::Streams::Buffer;
using winrt::Windows::Storage::Streams::IBuffer;
using winrt::Windows::Foundation::MemoryBuffer;
using winrt::Windows::Foundation::IMemoryBufferReference;
using Windows::Foundation::IMemoryBufferByteAccess;

void recognizeText(const cv::Mat &imageBinary,
		   std::function<void(std::string)> callback)
{
	cv::Mat padImage;
	cv::copyMakeBorder(imageBinary, padImage, 200, 200, 200, 200,
			   cv::BORDER_CONSTANT, cv::Scalar(255));

	int width = padImage.cols;
	int height = padImage.rows;
	const int dataSize = width * height;

	MemoryBuffer memoryBuffer(dataSize);

	IMemoryBufferReference ref = memoryBuffer.CreateReference();
	auto interop = ref.as<IMemoryBufferByteAccess>();
	uint8_t *value;
	uint32_t valueSize;
	winrt::check_hresult(interop->GetBuffer(&value, &valueSize));
	memcpy_s(value, valueSize, padImage.data, dataSize);

	Buffer buffer = Buffer::CreateCopyFromMemoryBuffer(memoryBuffer);
	buffer.Length(dataSize);

	SoftwareBitmap bitmap(BitmapPixelFormat::Gray8, width, height);
	bitmap.CopyFromBuffer(buffer);
	Language lang(L"ja");
	OcrEngine ocr = OcrEngine::TryCreateFromLanguage(lang);
	if (!ocr) {
		return "FAILED";
	}
	OcrResult result = ocr.RecognizeAsync(bitmap).get();
	callback(winrt::to_string(result.Text()));
}
