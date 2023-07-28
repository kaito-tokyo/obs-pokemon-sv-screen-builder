#include <filesystem>

#include <QTimer>

#include <obs-frontend-api.h>
#include <obs-module.h>
#include <util/config-file.h>

#include "UpdateDialog.hpp"
#include "GitHubClient.hpp"

#include "update-checker.h"

config_t *checkUpdateConfig = nullptr;
UpdateDialog *updateDialog = nullptr;

static bool getIsSkipping(config_t *config, std::string latestVersion)
{
	bool skip = config_get_bool(config, "check-update", "skip");
	const char *skipVersion =
		config_get_string(config, "check-update", "skip-version");
	if (skip) {
		if (skipVersion != nullptr && skipVersion == latestVersion) {
			return true;
		} else {
			config_set_bool(config, "check-update", "skip", false);
			config_save_safe(config, "tmp", nullptr);
			return false;
		}
	} else {
		return false;
	}
}

void update_checker_check_update(const char *latest_release_url,
				 const char *plugin_name,
				 const char *plugin_version)
{

	GitHubClient client(plugin_name, plugin_version);
	client.getLatestRelease(latest_release_url, [&](GitHubClient::LatestRelease
								result) {
		if (result.error) {
			blog(LOG_INFO,
			     "[%s] Failed to fetch latest release info!",
			     plugin_name);
			return;
		}
		if (result.version == plugin_version) {
			blog(LOG_INFO, "[%s] This plugin is latest!",
			     plugin_name);
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
			blog(LOG_ERROR,
			     "[%s] Update checker config cennot be opened!",
			     plugin_name);
			return;
		}
		if (getIsSkipping(checkUpdateConfig, result.version.c_str())) {
			blog(LOG_INFO, "[%s] Checking update skipped!",
			     plugin_name);
			return;
		}

		updateDialog = new UpdateDialog(
			result.version, result.body, checkUpdateConfig,
			(QWidget *)obs_frontend_get_main_window());
		QTimer::singleShot(2000, updateDialog, &UpdateDialog::exec);
	});
}

void update_checker_close(void)
{
	if (checkUpdateConfig) {
		config_close(checkUpdateConfig);
	}
	if (updateDialog) {
		delete updateDialog;
	}
}
