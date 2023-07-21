#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"

#include <string>

std::string recognizeTextImpl(const unsigned char *data, int width, int height) {
	return "aaa";
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
