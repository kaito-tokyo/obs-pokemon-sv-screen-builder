#pragma once

#include <string>

#include <curl/curl.h>

#include <obs.h>

#include "plugin-support.h"

namespace UpdateChecker {
static const std::string userAgent =
	std::string(PLUGIN_NAME) + "/" + PLUGIN_VERSION;

static std::size_t fetchStringFromUrlWriteFunc(void *ptr, std::size_t size,
					       size_t nmemb, std::string *data)
{
	data->append(static_cast<char *>(ptr), size * nmemb);
	return size * nmemb;
}

static std::string fetchStringFromUrl(const char *urlString)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		obs_log(LOG_INFO, "Failed to initialize curl");
		return "";
	}

	std::string responseBody;
	curl_easy_setopt(curl, CURLOPT_URL, urlString);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
			 fetchStringFromUrlWriteFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (code == CURLE_OK) {
		return responseBody;
	} else {
		obs_log(LOG_INFO, "Failed to get latest release info");
		return "";
	}
}
} // namespace UpdateChecker
