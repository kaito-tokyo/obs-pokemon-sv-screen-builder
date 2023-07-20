#include <QTimer>

#include <obs-frontend-api.h>
#include <util/config-file.h>

#include "UpdateDialog.hpp"
#include "GitHubClient.hpp"

#include "update-checker.h"

UpdateDialog *updateDialog;

static bool getIsSkipping(config_t *config, const char *pluginName,
			  const char *pluginVersion)
{
	bool skip = config_get_bool(config, pluginName, "check_update_skip");
	std::string skipVersion = config_get_string(
		config, pluginName, "check_update_skip_version");
	if (skip) {
		if (skipVersion == pluginVersion) {
			return true;
		} else {
			config_set_bool(config, pluginName, "check_update_skip",
					false);
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
	config_t *config = obs_frontend_get_global_config();
	if (getIsSkipping(config, plugin_name, plugin_version)) {
		blog(LOG_INFO, "[%s] Checking update skipped!", plugin_name);
		return;
	}

	GitHubClient client(plugin_name, plugin_version);
	auto result = client.getLatestRelease(latest_release_url);
	if (result.error) {
		blog(LOG_INFO, "[%s] Failed to fetch latest release info!",
		     plugin_name);
		return;
	}
	if (result.version == plugin_version) {
		blog(LOG_INFO, "[%s] This plugin is latest!", plugin_name);
		return;
	}

	updateDialog = new UpdateDialog(
		plugin_name, plugin_version, result.version.c_str(),
		result.body.c_str(), config,
		(QWidget *)obs_frontend_get_main_window());
	QTimer::singleShot(2000, updateDialog, &UpdateDialog::exec);
}
