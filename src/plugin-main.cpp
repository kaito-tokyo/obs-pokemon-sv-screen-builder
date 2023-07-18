#include <obs-module.h>

#include <plugin-support.h>

extern struct obs_source_info screen_info;
extern struct obs_source_info screen_my_selection_info;
extern struct obs_source_info screen_opponent_team_info;
extern struct obs_source_info screen_opponent_rank_info;
extern struct obs_source_info screen_my_rank_info;

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	obs_register_source(&screen_info);

	obs_log(LOG_INFO, "plugin loaded successfully (version %s)",
		PLUGIN_VERSION);
	return true;
}

void obs_module_unload()
{
	obs_log(LOG_INFO, "plugin unloaded");
}
