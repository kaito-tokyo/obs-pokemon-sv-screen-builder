#include <inttypes.h>

#include <opencv2/opencv.hpp>

#include <obs-module.h>
#include <util/platform.h>

#include "SceneDetector.h"

enum screen_state {
	STATE_UNKNOWN,
	STATE_ENTERING_SELECT_POKEMON,
	STATE_SELECT_POKEMON,
	STATE_ENTERING_CONFIRM_POKEMON,
	STATE_CONFIRM_POKEMON,
	STATE_ENTERING_MATCH,
	STATE_MATCH,
	STATE_ENTERING_RESULT,
	STATE_RESULT,
};

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

constexpr int N_POKEMONS = 6;

struct screen_context {
	obs_data_t *settings;
	obs_source_t *source;

	gs_texrender_t *texrender;
	gs_stagesurf_t *stagesurface;
	cv::Mat gameplay_bgra;

	uint64_t next_tick;
	SceneDetector sceneDetector;

	screen_state state;
	uint64_t last_state_change_ns;
	int my_selection_order_map[N_POKEMONS];
	SceneDetector::Scene prev_scene;
	uint64_t match_start_ns;
	uint64_t last_elapsed_seconds;
	uint64_t match_end_ns;

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

static uint64_t update_timer_text(obs_source_t *timer_source,
				  uint64_t time_start, uint64_t time_now,
				  uint64_t last_elapsed_seconds)
{
	uint64_t elapsed_seconds = (time_now - time_start) / 1000000000;
	if (elapsed_seconds == last_elapsed_seconds)
		return elapsed_seconds;
	uint64_t remaining_seconds = 20 * 60 - elapsed_seconds;
	uint64_t minutes = remaining_seconds / 60;
	uint64_t seconds = remaining_seconds % 60;

	char time_str[512];
	snprintf(time_str, sizeof(time_str), "%02" PRIu64 ":%02" PRIu64,
		 minutes, seconds);

	obs_data_t *settings = obs_data_create();
	obs_data_set_string(settings, "text", time_str);
	obs_source_update(timer_source, settings);
	obs_data_release(settings);

	return elapsed_seconds;
}

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

	if (context->state == STATE_UNKNOWN) {
		if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_ENTERING_SELECT_POKEMON;
			context->last_state_change_ns = os_gettime_ns();
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			blog(LOG_INFO, "State: UNKNOWN to ENTERING_SELECT");
		}
	} else if (context->state == STATE_ENTERING_SELECT_POKEMON) {
		const uint64_t now = os_gettime_ns();
		if (now - context->last_state_change_ns > 1000000000) {
			// pokemon_detector_sv_opponent_pokemon_crop(
			// 	context->detector_context);
			context->state = STATE_SELECT_POKEMON;
			blog(LOG_INFO,
			     "State: ENTERING_SELECT_POKEMON to SELECT_POKEMON");
		}
	} else if (context->state == STATE_SELECT_POKEMON) {
		// if (selection_order_detect_change(context)) {
		// 	export_selection_order_image(context);
		// }

		if (scene == SceneDetector::SCENE_UNDEFINED) {
			context->last_state_change_ns = os_gettime_ns();
			context->state = STATE_ENTERING_CONFIRM_POKEMON;
			blog(LOG_INFO,
			     "State: SELECT_POKEMON to ENTERING_CONFIRM_POKEMON");
		} else if (scene ==
			   SceneDetector::SCENE_BLACK_TRANSITION) {
			context->state = STATE_ENTERING_MATCH;
			blog(LOG_INFO,
			     "State: SELECT_POKEMON to ENTERING_MATCH");
		}
	} else if (context->state == STATE_ENTERING_CONFIRM_POKEMON) {
		uint64_t now = os_gettime_ns();
		if (now - context->last_state_change_ns > 500000000) {
			context->state = STATE_CONFIRM_POKEMON;
			blog(LOG_INFO,
			     "State: ENTERING_CONFIRM_POKEMON to CONFIRM_POKEMON");
		} else if (scene ==
			   SceneDetector::SCENE_BLACK_TRANSITION) {
			context->state = STATE_ENTERING_MATCH;
			blog(LOG_INFO,
			     "State: LEAVE_SELECT_POKEMON to ENTERING_MATCH");
		}
	} else if (context->state == STATE_CONFIRM_POKEMON) {
		if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_SELECT_POKEMON;
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			blog(LOG_INFO,
			     "State: CONFIRM_POKEMON to SELECT_POKEMON");
		} else if (scene ==
			   SceneDetector::SCENE_BLACK_TRANSITION) {
			context->state = STATE_ENTERING_MATCH;
			blog(LOG_INFO,
			     "State: CONFIRM_POKEMON to ENTERING_MATCH");
		}
	} else if (context->state == STATE_ENTERING_MATCH) {
		if (context->prev_scene !=
			    SceneDetector::SCENE_BLACK_TRANSITION &&
		    scene == SceneDetector::SCENE_BLACK_TRANSITION) {
			context->state = STATE_MATCH;
			context->match_start_ns = os_gettime_ns();
			blog(LOG_INFO, "State: ENTERING_MATCH to MATCH");
		} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_SELECT_POKEMON;
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			blog(LOG_INFO,
			     "State: ENTERING_MATCH to SELECT_POKEMON");
		}
	} else if (context->state == STATE_MATCH) {
		const char *timer_name =
			obs_data_get_string(context->settings, "timer_source");
		obs_source_t *timer_source = obs_get_source_by_name(timer_name);
		if (timer_source) {
			context->last_elapsed_seconds = update_timer_text(
				timer_source, context->match_start_ns,
				os_gettime_ns(), context->last_elapsed_seconds);
			obs_source_release(timer_source);
		}

		if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_ENTERING_SELECT_POKEMON;
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			blog(LOG_INFO,
			     "State: MATCH to ENTERING_SELECT_POKEMON");
		} else if (context->prev_scene !=
				   SceneDetector::SCENE_BLACK_TRANSITION &&
			   scene ==
				   SceneDetector::SCENE_BLACK_TRANSITION) {
			context->match_end_ns = os_gettime_ns();
			context->state = STATE_RESULT;
			blog(LOG_INFO, "MATCH to RESULT");
		}
	} else if (context->state == STATE_RESULT) {
		uint64_t now = os_gettime_ns();
		if (now - context->match_end_ns > 2000000000) {
			context->state = STATE_UNKNOWN;
			blog(LOG_INFO, "RESULT to UNKNOWN");
		} else if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			context->state = STATE_ENTERING_SELECT_POKEMON;
			blog(LOG_INFO, "MATCH to ENTERING_SELECT_POKEMON");
		}
	}
	context->prev_scene = scene;

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