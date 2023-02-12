#include <obs-module.h>

static const char *screen_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Pokemon SV Screen Builder";
}

static uint32_t screen_get_width(void *data) {
	UNUSED_PARAMETER(data);
	return 1920;
}

static uint32_t screen_get_height(void *data) {
	UNUSED_PARAMETER(data);
	return 1080;
}

static void screen_video_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(data);
	UNUSED_PARAMETER(effect);
}

struct obs_source_info screen_info = {
	.id = "obs-pokemon-sv-screen-builder",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = screen_get_name,
	// .create = filter_create,
	// .destroy = filter_destroy,
	.get_width = screen_get_width,
	.get_height = screen_get_height,
	.video_render = screen_video_render,
	// .video_tick = filter_video_tick,
	// .get_properties = filter_properties,
	// .get_defaults = filter_defaults,
};