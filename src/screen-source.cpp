#include <inttypes.h>
#include <sstream>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include <obs-module.h>
#include <obs-frontend-api.h>

#include "modules/Base64.hpp"
#include "modules/TextRecognizer.h"
#include "obs-browser-api.h"
#include "obs-platform-util.h"

#include "screen-source.h"

static void screen_main_render_callback(void *data, uint32_t cx, uint32_t cy)
{
	screen_context *context = reinterpret_cast<screen_context *>(data);

	if (!obs_source_enabled(context->source))
		return;

	const char *gameplay_name =
		obs_data_get_string(context->settings, "gameplay_source");
	obs_source_t *gameplay_source = obs_get_source_by_name(gameplay_name);
	if (!gameplay_source) {
		return;
	}
	if (!obs_source_enabled(gameplay_source)) {
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

	if (context->stagesurface) {
		uint32_t stagesurface_width =
			gs_stagesurface_get_width(context->stagesurface);
		uint32_t stagesurface_height =
			gs_stagesurface_get_height(context->stagesurface);
		if (stagesurface_width != gameplay_width ||
		    stagesurface_height != gameplay_height) {
			gs_stagesurface_destroy(context->stagesurface);
			context->stagesurface = nullptr;
		}
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
		} else {
			context->gameplay_bgra = cv::Mat(
				gameplay_height, gameplay_width, CV_8UC4);
			for (uint32_t i = 0; i < gameplay_height; i++) {
				memcpy(context->gameplay_bgra.data +
					       gameplay_width * 4 * i,
				       stagesurface_data + linesize * i,
				       gameplay_width * 4);
			}
		}
	}
	gs_stagesurface_unmap(context->stagesurface);

	UNUSED_PARAMETER(cx);
	UNUSED_PARAMETER(cy);
}

static const char *screen_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("PokemonSVScreenBuilder");
}

static void *screen_create(obs_data_t *settings, obs_source_t *source)
{
	void *rawContext = bmalloc(sizeof(screen_context));
	screen_context *context = new (rawContext) screen_context();
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
		bfree(context);
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

static void addBrowserSourceToSceneIfNotExists(obs_scene_t *scene,
					       const char *sourceName,
					       const char *moduleFileName,
					       int width, int height,
					       float posX, float posY,
					       float scaleX, float scaleY)
{
	obs_source_t *origSource = obs_get_source_by_name(sourceName);

	if (origSource) {
		obs_source_release(origSource);
		return;
	}

	obs_data_t *settings = obs_data_create();
	char *localFile = obs_module_file(moduleFileName);
	obs_data_set_string(settings, "local_file", localFile);
	obs_data_set_int(settings, "width", width);
	obs_data_set_int(settings, "height", height);
	obs_data_set_bool(settings, "is_local_file", true);
	obs_source_t *source = obs_source_create("browser_source", sourceName,
						 settings, nullptr);
	obs_scene_add(scene, source);
	obs_data_release(settings);
	bfree(localFile);

	obs_sceneitem_t *sceneItem =
		obs_scene_sceneitem_from_source(scene, source);
	vec2 pos{posX, posY};
	obs_sceneitem_set_pos(sceneItem, &pos);
	vec2 scale{scaleX, scaleY};
	obs_sceneitem_set_scale(sceneItem, &scale);
	obs_sceneitem_release(sceneItem);
}

static bool handleClickAddDefaultLayout(obs_properties_t *props,
					obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);

	obs_source_t *scene_source = obs_frontend_get_current_scene();
	obs_scene_t *scene = obs_scene_from_source(scene_source);

	addBrowserSourceToSceneIfNotExists(
		scene, obs_module_text("OpponentTeamSource"),
		"browser/OpponentTeam.html", 104, 664, 1751, 0, 1.625, 1.625);

	addBrowserSourceToSceneIfNotExists(scene,
					   obs_module_text("MySelectionSource"),
					   "browser/MySelection.html", 1587,
					   108, 0, 972, 1, 1);

	obs_scene_release(scene);
	return true;
}

static obs_properties_t *screen_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_properties_t *props = obs_properties_create();

	obs_property_t *prop_gameplay_source = obs_properties_add_list(
		props, "gameplay_source", obs_module_text("GamePlaySource"),
		OBS_COMBO_TYPE_EDITABLE, OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_all_sources_to_list, prop_gameplay_source);

	obs_property_t *prop_timer = obs_properties_add_list(
		props, "timer_source", obs_module_text("TimerSource"),
		OBS_COMBO_TYPE_EDITABLE, OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_text_sources_to_list, prop_timer);

	obs_property_t *prop_opponent_rank_source = obs_properties_add_list(
		props, "opponent_rank_source",
		obs_module_text("OpponentRankSource"), OBS_COMBO_TYPE_EDITABLE,
		OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_text_sources_to_list, prop_opponent_rank_source);

	obs_properties_add_button(
		props, "add_default_layout_button",
		obs_module_text("AddDefaultLayoutDescription"),
		&handleClickAddDefaultLayout);

	return props;
}

static void drawOpponentPokemons(screen_context *context)
{
	context->opponentPokemonCropper.crop(context->gameplay_bgra);
	context->opponentPokemonCropper.generateMask();
	std::vector<std::string> imageUrls(N_POKEMONS);
	for (int i = 0; i < N_POKEMONS; i++) {
		std::vector<uchar> pngImage;
		cv::imencode(".png",
			     context->opponentPokemonCropper.imagesBGRA[i],
			     pngImage);
		imageUrls[i] =
			"data:image/png;base64," + Base64::encode(pngImage);
	}
	nlohmann::json json{
		{"imageUrls", imageUrls},
	};
	const char eventName[] = "obsPokemonSvScreenBuilderOpponentTeamShown";
	std::string jsonString = json.dump();
	sendEventToAllBrowserSources(eventName, jsonString.c_str());
}

static bool detectSelectionOrderChange(screen_context *context)
{
	context->selectionOrderCropper.crop(context->gameplay_bgra);

	int orders[N_POKEMONS];
	bool change_detected = false;
	for (int i = 0; i < N_POKEMONS; i++) {
		orders[i] = context->selectionRecognizer.recognizeSelection(
			context->selectionOrderCropper.imagesBGR[i]);
		if (orders[i] > 0 &&
		    context->my_selection_order_map[orders[i] - 1] != i + 1) {
			context->my_selection_order_map[orders[i] - 1] = i + 1;
			change_detected = true;
		}
	}
	if (change_detected) {
		blog(LOG_INFO, "My order: %d %d %d %d %d %d\n", orders[0],
		     orders[1], orders[2], orders[3], orders[4], orders[5]);
	}
	return change_detected;
}

static void drawMyPokemons(screen_context *context)
{
	std::vector<std::string> imageUrls(N_POKEMONS);
	context->myPokemonCropper.crop(context->gameplay_bgra);
	for (int i = 0; i < N_POKEMONS; i++) {
		cv::Vec4b &pixel =
			context->myPokemonCropper.imagesBGRA[i].at<cv::Vec4b>(
				0, 0);
		if (pixel[1] > 150 && pixel[2] > 150)
			continue;
		context->myPokemonsBGRA[i] =
			context->myPokemonCropper.imagesBGRA[i].clone();
	}

	for (int i = 0; i < N_POKEMONS; i++) {
		if (context->myPokemonsBGRA[i].empty()) {
			imageUrls[i] = "";
		} else {
			std::vector<uchar> pngImage;
			cv::imencode(".png", context->myPokemonsBGRA[i],
				     pngImage);
			imageUrls[i] = "data:image/png;base64," +
				       Base64::encode(pngImage);
		}
	}

	nlohmann::json json{
		{"imageUrls", imageUrls},
		{"mySelectionOrderMap", context->my_selection_order_map},
	};
	const char eventName[] = "obsPokemonSvScreenBuilderMySelectionChanged";
	std::string jsonString = json.dump();
	sendEventToAllBrowserSources(eventName, jsonString.c_str());
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

static std::string update_text(obs_source_t *source, std::string rank)
{
	obs_data_t *settings = obs_data_create();
	obs_data_set_string(settings, "text", rank.c_str());
	obs_source_update(source, settings);
	obs_data_release(settings);
	return rank;
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
		cv::Mat screenTextBinary =
			context->sceneDetector.generateTextBinaryScreen(
				context->gameplay_bgra);

		if (context->sceneDetector.isOpponentRankShown(
			    screenTextBinary)) {
			context->opponentRankExtractor.extract(
				screenTextBinary, context->gameplay_bgra);
			std::string result = recognizeText(
				context->opponentRankExtractor.imageBGRA);
			nlohmann::json json{
				{"text", result},
			};
			const char *eventName =
				"obsPokemonSvScreenBuilderOpponentRankShown";
			std::string jsonString(json.dump());
			sendEventToAllBrowserSources(eventName,
						     jsonString.c_str());
		}

		if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_ENTERING_SELECT_POKEMON;
			context->last_state_change_ns = os_gettime_ns();
			blog(LOG_INFO, "State: UNKNOWN to ENTERING_SELECT");
		}
	} else if (context->state == STATE_ENTERING_SELECT_POKEMON) {
		const uint64_t now = os_gettime_ns();
		if (now - context->last_state_change_ns > 1000000000) {
			drawOpponentPokemons(context);
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			context->state = STATE_SELECT_POKEMON;
			blog(LOG_INFO,
			     "State: ENTERING_SELECT_POKEMON to SELECT_POKEMON");
		}
	} else if (context->state == STATE_SELECT_POKEMON) {
		if (detectSelectionOrderChange(context)) {
			drawMyPokemons(context);
		}

		if (scene == SceneDetector::SCENE_UNDEFINED) {
			context->last_state_change_ns = os_gettime_ns();
			context->state = STATE_ENTERING_CONFIRM_POKEMON;
			blog(LOG_INFO,
			     "State: SELECT_POKEMON to ENTERING_CONFIRM_POKEMON");
		} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
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
		} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
			context->state = STATE_ENTERING_MATCH;
			blog(LOG_INFO,
			     "State: LEAVE_SELECT_POKEMON to ENTERING_MATCH");
		}
	} else if (context->state == STATE_CONFIRM_POKEMON) {
		if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_ENTERING_SELECT_POKEMON;
			blog(LOG_INFO,
			     "State: CONFIRM_POKEMON to STATE_ENTERING_SELECT_POKEMON");
		} else if (scene == SceneDetector::SCENE_BLACK_TRANSITION) {
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
			context->state = STATE_ENTERING_SELECT_POKEMON;
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
			blog(LOG_INFO,
			     "State: MATCH to ENTERING_SELECT_POKEMON");
		} else if (context->prev_scene !=
				   SceneDetector::SCENE_BLACK_TRANSITION &&
			   scene == SceneDetector::SCENE_BLACK_TRANSITION) {
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
