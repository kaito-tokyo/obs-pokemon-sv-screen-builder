#pragma once

#include <string>
#include <vector>

namespace Base64 {
std::string encode(const std::vector<unsigned char> input)
{
	const char table[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::vector<char> encoded((input.size() + 2) / 3 * 4 + 1);

	size_t i = 0, j = 0;
	for (; j < input.size() - 2; i += 4, j += 3) {
		encoded[i] = table[input[j] >> 2];
		encoded[i + 1] =
			table[((input[j] << 4) & 0x30) | input[j + 1] >> 4];
		encoded[i + 2] =
			table[((input[j + 1] << 2) & 0x3C) | input[j + 2] >> 6];
		encoded[i + 3] = table[input[j + 2] & 0x3F];
	}

	size_t pad = input.size() % 3;
	if (pad == 0) {
		encoded[i] = '\0';
	} else if (pad == 1) {
		encoded[i] = table[input[j] >> 2];
		encoded[i + 1] = table[(input[j] << 4) & 0x30];
		encoded[i + 2] = '=';
		encoded[i + 3] = '=';
		encoded[i + 4] = '\0';
	} else if (pad == 2) {
		encoded[i] = table[input[j] >> 2];
		encoded[i + 1] =
			table[((input[j] << 4) & 0x30) | input[j + 1] >> 4];
		encoded[i + 2] = table[(input[j + 1] << 2) & 0x3C];
		encoded[i + 3] = '=';
		encoded[i + 4] = '\0';
	}

	return std::string(encoded.begin(), encoded.end());
}
}
