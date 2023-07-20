#include <obs-module.h>

#include <plugin-support.h>

const char *screen_get_name(void *unused);
void *screen_create(obs_data_t *settings, obs_source_t *source);
void screen_destroy(void *data);
void screen_defaults(obs_data_t *settings);
obs_properties_t *screen_properties(void *data);
void screen_update(void *data, obs_data_t *settings);
void screen_video_tick(void *data, float seconds);

static struct obs_source_info screen_info = {
	.id = "obs-pokemon-sv-screen-builder",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO,
	.get_name = screen_get_name,
	.create = screen_create,
	.destroy = screen_destroy,
	.get_defaults = screen_defaults,
	.get_properties = screen_properties,
	.update = screen_update,
	//.video_tick = screen_video_tick,
	.load = screen_update,
};

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
