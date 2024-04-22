#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <obs.h>

#include "plugin-support.h"

#include "CurlClient.hpp"

class GitHubClient {
public:
	struct LatestRelease {
		std::string version;
		std::string body;
		bool error;
	};

	void getLatestRelease(const char *latestReleaseUrl,
			      std::function<void(LatestRelease)> callback) const
	{
		auto cb = [callback](std::string responseBody, int errorCode) {
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
					"Failed to parse the latest release info! %s",
					responseBody.c_str());
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
		};
		fetchStringFromUrl(latestReleaseUrl, cb);
	}
};
