#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"

#include <winrt/Windows.Media.Ocr.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.h>
#include <MemoryBuffer.h>
#include <winrt/Windows.Globalization.h>

#include <string>

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

#include <fstream>
std::string recognizeTextImpl(const unsigned char *data, int width, int height) {
	const int dataSize = width * height;

	MemoryBuffer memoryBuffer(dataSize);

	IMemoryBufferReference ref = memoryBuffer.CreateReference();
	memcpy_s(ref.data(), dataSize, data, dataSize);

	std::ofstream ofs("C:\\Users\\User\\a.txt");

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

namespace winrt::WinRTTextRecognizer::implementation
{
    int32_t Class::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void Class::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
