#pragma once

#include <string>

#include <obs.h>

#include <curl/curl.h>

class GitHubClient {
public:
	using URLResponse = std::pair<std::string, CURLcode>;

	struct LatestRelease {
		std::string version;
		std::string body;
		bool error;
	};

	GitHubClient(const char *_pluginName, const char *_pluginVersion)
		: pluginName(_pluginName), pluginVersion(_pluginVersion)
	{
	}

	LatestRelease getLatestRelease(const char *latestReleaseUrl) const
	{
		URLResponse response = getUrl(latestReleaseUrl);
		if (response.second != CURLE_OK) {
			blog(LOG_INFO,
			     "[%s] Failed to get the latest release info!",
			     pluginName.c_str());
			return {"", "", true};
		}

		obs_data_t *data =
			obs_data_create_from_json(response.first.c_str());
		if (!data) {
			blog(LOG_INFO,
			     "[%s] Failed to parse the latest release info!",
			     pluginName.c_str());
			return {"", "", true};
		}

		LatestRelease result;
		result.version = obs_data_get_string(data, "tag_name");
		result.body = obs_data_get_string(data, "body");
		result.error = false;
		obs_data_release(data);

		return result;
	}

private:
	const std::string pluginName;
	const std::string pluginVersion;
	const std::string userAgent = pluginName + "/" + pluginVersion;
	const std::string logPrefix = "[" + pluginName + "]";

	static size_t write(void *ptr, size_t size, size_t nmemb,
			    std::string *data)
	{
		data->append(static_cast<char *>(ptr), size * nmemb);
		return size * nmemb;
	}

	std::pair<std::string, CURLcode> getUrl(const char *url) const
	{
		CURL *curl = curl_easy_init();
		if (!curl) {
			blog(LOG_ERROR, "[%s] Failed to initialize curl!",
			     pluginName.c_str());
			return {"", CURL_LAST};
		}

		CURLcode code;
		std::string data;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

		code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (code == CURLE_OK) {
			return {data, code};
		} else {
			blog(LOG_ERROR,
			     "[%s] Failed to fetch a content from %s!",
			     pluginName.c_str(), url);
			return {"", code};
		}
	}
};
