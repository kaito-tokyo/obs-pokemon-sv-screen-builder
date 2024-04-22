#pragma once

#include <string>

#include <obs.h>

#include "plugin-support.h"

#include "CurlClient.hpp"

namespace UpdateChecker {
std::string getLatestReleaseVersion(const char *latestReleaseUrl)
{
	std::string responseBody = fetchStringFromUrl(latestReleaseUrl);
	if (responseBody.empty()) {
		obs_log(LOG_INFO, "Failed to get the latest release info!");
		return "";
	}

	obs_data_t *data = obs_data_create_from_json(responseBody.c_str());
	const char *rawVersion = obs_data_get_string(data, "tag_name");
	if (!rawVersion) {
		obs_log(LOG_INFO, "Malformed JSON from GitHub!");
		return "";
	}
	std::string version = rawVersion;
	obs_data_release(data);
	return version;
}
} // namespace UpdateChecker
