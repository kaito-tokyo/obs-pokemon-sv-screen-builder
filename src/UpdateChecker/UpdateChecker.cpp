#include <filesystem>

#include <QTimer>

#include <obs-frontend-api.h>
#include <obs-module.h>
#include <util/config-file.h>

#include "UpdateDialog.hpp"
#include "GitHubClient.hpp"
#include <plugin-support.h>

#include "UpdateChecker.h"

config_t *checkUpdateConfig = nullptr;
UpdateDialog *updateDialog = nullptr;

static bool getIsSkipping(config_t *config, std::string latestVersion)
{
	if (latestVersion == PLUGIN_VERSION) {
		return true;
	}

	const char *skipVersion =
		config_get_string(config, "check-update", "skip-version");
	return skipVersion != nullptr && skipVersion == latestVersion;
}

void update_checker_check_update(const char *latest_release_url)
{
	std::string currentTag = std::string("v") + PLUGIN_VERSION;
	GitHubClient client;
	auto cb = [currentTag](GitHubClient::LatestRelease result) {
		if (result.error) {
			obs_log(LOG_INFO,
				"Failed to fetch latest release info!");
			return;
		}
		if (result.version == currentTag) {
			obs_log(LOG_INFO, "This plugin is latest!");
			return;
		}

		char *configDirDstr = obs_module_config_path("");
		std::filesystem::create_directories(configDirDstr);
		bfree(configDirDstr);

		char *configDstr = obs_module_config_path("update-checker.ini");
		int configResult = config_open(&checkUpdateConfig, configDstr,
					       CONFIG_OPEN_ALWAYS);
		bfree(configDstr);
		if (configResult != CONFIG_SUCCESS) {
			obs_log(LOG_ERROR,
				"Update checker config cennot be opened!");
			return;
		}
		if (getIsSkipping(checkUpdateConfig, result.version.c_str())) {
			obs_log(LOG_INFO, "Checking update skipped!");
			return;
		}

		obs_log(LOG_INFO, "Update available!");

		updateDialog = new UpdateDialog(
			result.version, result.body, checkUpdateConfig,
			(QWidget *)obs_frontend_get_main_window());
		QTimer::singleShot(2000, updateDialog, &UpdateDialog::exec);
	};
	client.getLatestRelease(latest_release_url, cb);
}

void update_checker_close(void)
{
	if (checkUpdateConfig) {
		config_close(checkUpdateConfig);
	}
}
