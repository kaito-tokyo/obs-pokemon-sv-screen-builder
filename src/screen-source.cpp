#include <opencv2/opencv.hpp>

#include <obs-module.h>
#include <util/platform.h>

#include "SceneDetector.h"

const HistClassifier classifier_lobby_my_select = {.rangeCol = {149, 811},
						   .rangeRow = {139, 842},
						   .histChannel = 0,
						   .histBins = 30,
						   .histMaxIndex = 17,
						   .histRatio = 0.5};
const HistClassifier classifier_lobby_opponent_select = {.rangeCol = {1229,
								      1649},
							 .rangeRow = {227, 836},
							 .histChannel = 0,
							 .histBins = 30,
							 .histMaxIndex = 0,
							 .histRatio = 0.8};
const HistClassifier classifier_black_transition = {.rangeCol = {400, 600},
						    .rangeRow = {400, 600},
						    .histChannel = 2,
						    .histBins = 8,
						    .histMaxIndex = 0,
						    .histRatio = 0.8};

struct screen_context {
	obs_data_t *settings;
	obs_source_t *source;

	gs_texrender_t *texrender;
	gs_stagesurf_t *stagesurface;
	cv::Mat gameplay_bgra;

	uint64_t next_tick;
	SceneDetector sceneDetector;

	screen_context()
		: sceneDetector(classifier_lobby_my_select,
				classifier_lobby_opponent_select,
				classifier_black_transition)
	{
	}
};

static void screen_main_render_callback(void *data, uint32_t cx, uint32_t cy)
{
	screen_context *context = reinterpret_cast<screen_context *>(data);

	if (!obs_source_enabled(context->source))
		return;

	const char *gameplay_name =
		obs_data_get_string(context->settings, "gameplay_source");
	obs_source_t *gameplay_source = obs_get_source_by_name(gameplay_name);
	if (!gameplay_source || !obs_source_enabled(gameplay_source)) {
		obs_source_release(gameplay_source);
		return;
	}

	const uint32_t gameplay_width = obs_source_get_width(gameplay_source);
	const uint32_t gameplay_height = obs_source_get_height(gameplay_source);

	if (gameplay_width == 0 || gameplay_height == 0) {
		obs_source_release(gameplay_source);
		return;
	}

	gs_texrender_reset(context->texrender);
	if (!gs_texrender_begin(context->texrender, gameplay_width,
				gameplay_height)) {
		obs_source_release(gameplay_source);
		return;
	}
	gs_ortho(0.0f, static_cast<float>(gameplay_width), 0.0f,
		 static_cast<float>(gameplay_height), -100.0f, 100.0f);
	obs_source_video_render(gameplay_source);
	gs_texrender_end(context->texrender);

	obs_source_release(gameplay_source);

	uint32_t stagesurface_width =
		gs_stagesurface_get_width(context->stagesurface);
	uint32_t stagesurface_height =
		gs_stagesurface_get_height(context->stagesurface);
	if (context->stagesurface && (stagesurface_width != gameplay_width ||
				      stagesurface_height != gameplay_height)) {
		gs_stagesurface_destroy(context->stagesurface);
		context->stagesurface = nullptr;
	}
	if (!context->stagesurface) {
		context->stagesurface = gs_stagesurface_create(
			gameplay_width, gameplay_height, GS_BGRA);
	}
	gs_stage_texture(context->stagesurface,
			 gs_texrender_get_texture(context->texrender));
	uint8_t *stagesurface_data;
	uint32_t linesize;
	if (!gs_stagesurface_map(context->stagesurface, &stagesurface_data,
				 &linesize))
		return;
	if (stagesurface_data && linesize) {
		if (gameplay_width * 4 == linesize) {
			context->gameplay_bgra =
				cv::Mat(gameplay_height, gameplay_width,
					CV_8UC4, stagesurface_data);
		}
	}
	gs_stagesurface_unmap(context->stagesurface);

	UNUSED_PARAMETER(cx);
	UNUSED_PARAMETER(cy);
}

static const char *screen_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Pokemon SV Screen Builder";
}

static void *screen_create(obs_data_t *settings, obs_source_t *source)
{
	screen_context *context = new screen_context();
	context->settings = settings;
	context->source = source;

	context->texrender = gs_texrender_create(GS_BGRA, GS_ZS_NONE);

	obs_add_main_render_callback(screen_main_render_callback, context);

	UNUSED_PARAMETER(settings);
	return context;
}

static void screen_destroy(void *data)
{
	screen_context *context = reinterpret_cast<screen_context *>(data);

	if (context) {
		obs_remove_main_render_callback(screen_main_render_callback,
						context);
		delete context;
	}
}

static void screen_defaults(obs_data_t *settings)
{
	obs_data_set_default_string(settings, "gameplay_source", "");
	obs_data_set_default_string(settings, "timer_source", "");
}

static bool add_all_sources_to_list(void *param, obs_source_t *source)
{
	obs_property_t *prop = reinterpret_cast<obs_property_t *>(param);
	const char *name = obs_source_get_name(source);
	obs_property_list_add_string(prop, name, name);
	return true;
}

static bool add_text_sources_to_list(void *param, obs_source_t *source)
{
	obs_property_t *prop = reinterpret_cast<obs_property_t *>(param);
	const char *id = obs_source_get_id(source);
	if (strcmp(id, "text_gdiplus_v2") == 0 ||
	    strcmp(id, "text_gdiplus") == 0 ||
	    strcmp(id, "text_ft2_source") == 0 ||
	    strcmp(id, "text_ft2_source_v2") == 0) {
		const char *name = obs_source_get_name(source);
		obs_property_list_add_string(prop, name, name);
	}
	return true;
}

static obs_properties_t *screen_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_properties_t *props = obs_properties_create();

	obs_property_t *prop_gameplay_source = obs_properties_add_list(
		props, "gameplay_source", "Game Play Source",
		OBS_COMBO_TYPE_EDITABLE, OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_all_sources_to_list, prop_gameplay_source);

	obs_property_t *prop_timer = obs_properties_add_list(
		props, "timer_source", "Timer Source", OBS_COMBO_TYPE_EDITABLE,
		OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_text_sources_to_list, prop_timer);

	return props;
}

#include <iostream>
static void screen_video_tick(void *data, float seconds)
{
	screen_context *context = reinterpret_cast<screen_context *>(data);
	uint64_t cur_time = os_gettime_ns();

	if (cur_time < context->next_tick + 1000 * 1000 * 100)
		return;
	context->next_tick = cur_time + 1000 * 1000 * 100;

	if (context->gameplay_bgra.empty())
		return;

	cv::Mat gameplay_bgr, gameplay_hsv;
	cv::cvtColor(context->gameplay_bgra, gameplay_bgr, cv::COLOR_BGRA2BGR);
	cv::cvtColor(gameplay_bgr, gameplay_hsv, cv::COLOR_BGR2HSV);
	SceneDetector::Scene scene =
		context->sceneDetector.detectScene(gameplay_hsv);
	blog(LOG_INFO, "Detected scene: %d\n", scene);

	struct obs_source_frame frame = {
		.width = static_cast<uint32_t>(context->gameplay_bgra.cols),
		.height = static_cast<uint32_t>(context->gameplay_bgra.rows),
		.timestamp = cur_time,
		.format = VIDEO_FORMAT_BGRA,
	};
	frame.data[0] = context->gameplay_bgra.data;
	frame.linesize[0] = context->gameplay_bgra.cols * 4;

	obs_source_output_video(context->source, &frame);

	UNUSED_PARAMETER(seconds);
}

struct obs_source_info screen_info = {
	.id = "obs-pokemon-sv-screen-builder",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO,
	.get_name = screen_get_name,
	.create = screen_create,
	.destroy = screen_destroy,
	.get_defaults = screen_defaults,
	.get_properties = screen_properties,
	.video_tick = screen_video_tick,
};