#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

#include <obs.h>
#include "plugin-support.h"

#ifdef __APPLE__
void fetchStringFromUrl(const char *urlString,
			std::function<void(std::string, int)> callback);
#else
#include <curl/curl.h>
#endif

class GitHubClient {
public:
	using URLResponse = std::pair<std::string, int>;

	struct LatestRelease {
		std::string version;
		std::string body;
		bool error;
	};

	GitHubClient(const char *_pluginName, const char *_pluginVersion)
		: pluginName(_pluginName), pluginVersion(_pluginVersion)
	{
	}

	void getLatestRelease(const char *latestReleaseUrl,
			      std::function<void(LatestRelease)> callback) const
	{
		getUrl(latestReleaseUrl, [this,
					  callback](std::string responseBody,
						    int errorCode) {
			if (errorCode != 0) {
				obs_log(LOG_INFO,
					"Failed to get the latest release info!");
				callback({"", "", true});
				return;
			}

			obs_data_t *data =
				obs_data_create_from_json(responseBody.c_str());
			if (!data) {
				obs_log(LOG_INFO,
					"Failed to parse the latest release info!");
				callback({"", "", true});
				return;
			}

			LatestRelease result;
			const char *version =
				obs_data_get_string(data, "tag_name");
			const char *body = obs_data_get_string(data, "body");
			if (!version || !body) {
				obs_log(LOG_INFO,
					"Malformed JSON from GitHub!");
				result.error = true;
			} else {
				result.version =
					obs_data_get_string(data, "tag_name");
				result.body = obs_data_get_string(data, "body");
				result.error = false;
			}
			obs_data_release(data);
			callback(result);
		});
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

	void getUrl(const char *url,
		    std::function<void(std::string, int)> callback) const
	{
#ifdef __APPLE__
		fetchStringFromUrl(url, callback);
#else
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
#endif
	}
};
