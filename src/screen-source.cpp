#include <inttypes.h>
#include <sstream>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>

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

static std::string getFrontendRecordPath(config_t *config)
{
	std::string output = config_get_string(config, "Output", "Mode");
	if (output == "Advanced" || output == "advanced") {
		std::string advOut =
			config_get_string(config, "AdvOut", "RecType");
		if (advOut == "Standard" || advOut == "standard") {
			return config_get_string(config, "AdvOut",
						 "RecFilePath");
		} else {
			return config_get_string(config, "AdvOut",
						 "FFFilePath");
		}
	} else {
		return config_get_string(config, "SimpleOutput", "FilePath");
	}
}

static void screen_defaults(obs_data_t *settings)
{
	obs_data_set_default_string(settings, "gameplay_source", "");
	obs_data_set_default_string(settings, "timer_source", "");
	config_t *config = obs_frontend_get_profile_config();
	const std::string recordPath = getFrontendRecordPath(config);
	const std::string logPath =
		recordPath + "/obs-pokemon-sv-screen-builder-log";
	obs_data_set_default_string(settings, "log_path", logPath.c_str());
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
		"browser/OpponentTeam.html", 78, 488, 1751, 0, 2.21f, 2.21f);

	addBrowserSourceToSceneIfNotExists(scene,
					   obs_module_text("MySelectionSource"),
					   "browser/MySelection.html", 1587,
					   108, 0, 972, 1, 1);

	addBrowserSourceToSceneIfNotExists(
		scene, obs_module_text("OpponentRankSource"),
		"browser/OpponentRank.html", 600, 100, 0, 864, 1, 1);

	addBrowserSourceToSceneIfNotExists(scene,
					   obs_module_text("MatchTimerSource"),
					   "browser/MatchTimer.html", 530, 100,
					   600, 872, 1, 1);

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

	obs_properties_t *props_log = obs_properties_create();
	obs_properties_add_path(props_log, "log_path",
				obs_module_text("LogPath"), OBS_PATH_DIRECTORY,
				NULL, NULL);
	obs_properties_add_group(props, "log_enabled",
				 obs_module_text("LogEnabled"),
				 OBS_GROUP_CHECKABLE, props_log);

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

	cv::Mat gameplay_bgr, gameplay_hsv, gameplay_gray, gameplay_binary;
	cv::cvtColor(context->gameplay_bgra, gameplay_bgr, cv::COLOR_BGRA2BGR);
	cv::cvtColor(gameplay_bgr, gameplay_hsv, cv::COLOR_BGR2HSV);
	cv::cvtColor(context->gameplay_bgra, gameplay_gray,
		     cv::COLOR_BGRA2GRAY);
	cv::threshold(gameplay_gray, gameplay_binary, 128, 255,
		      cv::THRESH_BINARY);
	SceneDetector::Scene scene = context->sceneDetector.detectScene(
		gameplay_hsv, gameplay_binary);

	ScreenState nextState = ScreenState::UNKNOWN;
	if (context->state == ScreenState::UNKNOWN) {
		nextState = handleUnknown(scene);
	} else if (context->state == ScreenState::ENTERING_SHOW_RANK) {
		nextState = handleEnteringShowRank(
			context->opponentRankExtractor, gameplay_binary);
	} else if (context->state == ScreenState::SHOW_RANK) {
		nextState = handleShowRank(scene);
	} else if (context->state == ScreenState::ENTERING_SELECT_POKEMON) {
		nextState = handleEnteringSelectPokemon(
			context->last_state_change_ns, context->gameplay_bgra,
			context->opponentPokemonCropper,
			context->my_selection_order_map);
	} else if (context->state == ScreenState::SELECT_POKEMON) {
		nextState = handleSelectPokemon(
			scene, context->selectionOrderCropper,
			context->gameplay_bgra, context->selectionRecognizer,
			context->my_selection_order_map,
			context->myPokemonCropper, context->myPokemonsBGRA);
	} else if (context->state == ScreenState::ENTERING_CONFIRM_POKEMON) {
		nextState = handleEnteringConfirmPokemon(
			scene, context->last_state_change_ns);
	} else if (context->state == ScreenState::CONFIRM_POKEMON) {
		nextState = handleConfirmPokemon(scene);
	} else if (context->state == ScreenState::ENTERING_MATCH) {
		nextState = handleEnteringMatch(scene, context->prev_scene);
	} else if (context->state == ScreenState::MATCH) {
		nextState = handleMatch(scene, context->prev_scene);
	} else if (context->state == ScreenState::ENTERING_RESULT) {
		nextState = handleEnteringResult();
	} else if (context->state == ScreenState::RESULT) {
		nextState = handleResult(scene, context->last_state_change_ns);
	}
	if (nextState != context->state) {
		context->last_state_change_ns = os_gettime_ns();
		blog(LOG_INFO, "State: %s to %s",
		     ScreenStateNames.at(context->state),
		     ScreenStateNames.at(nextState));
		context->state = nextState;
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
