#include "pch.h"

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

std::string recognizeTextImpl(const unsigned char *data, int width, int height)
{
	const int dataSize = width * height;

	MemoryBuffer memoryBuffer(dataSize);

	IMemoryBufferReference ref = memoryBuffer.CreateReference();
	memcpy_s(ref.data(), dataSize, data, dataSize);

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
	return winrt::to_string(result.Text());
}
