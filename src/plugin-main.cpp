/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>

#include "plugin-macros.generated.h"

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

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	obs_register_source(&screen_info);

	blog(LOG_INFO, "plugin loaded successfully (version %s)",
	     PLUGIN_VERSION);
	return true;
}

void obs_module_unload()
{
	blog(LOG_INFO, "plugin unloaded");
}
