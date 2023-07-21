#include <inttypes.h>
#include <sstream>
#include <iomanip>
#include <filesystem>

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
#include "plugin-support.h"

static void screen_main_render_callback(void *data, uint32_t cx, uint32_t cy)
{
	screen_context *context = static_cast<screen_context *>(data);

	if (!obs_source_enabled(context->source))
		return;

	if (!context->gameplaySource) {
		return;
	}

	const uint32_t gameplay_width =
		obs_source_get_width(context->gameplaySource);
	const uint32_t gameplay_height =
		obs_source_get_height(context->gameplaySource);

	if (gameplay_width == 0 || gameplay_height == 0) {
		return;
	}

	gs_texrender_reset(context->texrender);
	if (!gs_texrender_begin(context->texrender, gameplay_width,
				gameplay_height)) {
		return;
	}
	gs_ortho(0.0f, static_cast<float>(gameplay_width), 0.0f,
		 static_cast<float>(gameplay_height), -100.0f, 100.0f);
	obs_source_video_render(context->gameplaySource);
	gs_texrender_end(context->texrender);

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

extern "C" const char *screen_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("PokemonSVScreenBuilder");
}

extern "C" void *screen_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);

	void *rawContext = bmalloc(sizeof(screen_context));
	screen_context *context = new (rawContext) screen_context();
	context->source = source;

	context->texrender = gs_texrender_create(GS_BGRA, GS_ZS_NONE);

	obs_add_main_render_callback(screen_main_render_callback, context);

	return context;
}

extern "C" void screen_destroy(void *data)
{
	screen_context *context = static_cast<screen_context *>(data);

	obs_enter_graphics();
	gs_texrender_destroy(context->texrender);
	if (context->gameplaySource) {
		obs_source_release(context->gameplaySource);
	}
	if (context->stagesurface) {
		gs_stagesurface_destroy(context->stagesurface);
	}
	obs_leave_graphics();

	obs_remove_main_render_callback(screen_main_render_callback, context);
	context->~screen_context();
	bfree(context);
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

extern "C" void screen_defaults(obs_data_t *settings)
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
	obs_property_t *prop = static_cast<obs_property_t *>(param);
	const char *name = obs_source_get_name(source);
	obs_property_list_add_string(prop, name, name);
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
	std::filesystem::path localFileAbs =
		std::filesystem::absolute(localFile);
	bfree(localFile);
	std::string localFileString = localFileAbs.string<char>();

	obs_data_set_string(settings, "local_file", localFileString.c_str());
	obs_data_set_int(settings, "width", width);
	obs_data_set_int(settings, "height", height);
	obs_data_set_bool(settings, "is_local_file", true);
	obs_source_t *source = obs_source_create("browser_source", sourceName,
						 settings, nullptr);
	obs_scene_add(scene, source);
	obs_source_release(source);
	obs_data_release(settings);

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
					   obs_module_text("MyRankSource"),
					   "browser/MyRank.html", 600, 100, 0,
					   764, 1, 1);

	addBrowserSourceToSceneIfNotExists(scene,
					   obs_module_text("MatchTimerSource"),
					   "browser/MatchTimer.html", 530, 100,
					   600, 872, 1, 1);

	obs_scene_release(scene);

	screen_context *context = static_cast<screen_context *>(data);
	context->defaultLayoutCreatedDialog.exec();
	return true;
}

extern "C" obs_properties_t *screen_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_button(
		props, "add_default_layout_button",
		obs_module_text("AddDefaultLayoutDescription"),
		&handleClickAddDefaultLayout);

	obs_property_t *prop_gameplay_source = obs_properties_add_list(
		props, "gameplay_source", obs_module_text("GamePlaySource"),
		OBS_COMBO_TYPE_EDITABLE, OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_all_sources_to_list, prop_gameplay_source);

	obs_properties_t *props_log = obs_properties_create();
	obs_properties_add_path(props_log, "log_path",
				obs_module_text("LogPath"), OBS_PATH_DIRECTORY,
				NULL, NULL);
	obs_properties_add_group(props, "log_enabled",
				 obs_module_text("LogEnabled"),
				 OBS_GROUP_CHECKABLE, props_log);

	return props;
}

extern "C" void screen_update(void *data, obs_data_t *settings)
{
	screen_context *context = static_cast<screen_context *>(data);
	bool logEnabled = obs_data_get_bool(settings, "log_enabled");
	if (logEnabled) {
		const char *logPath = obs_data_get_string(settings, "log_path");
		context->logger.basedir = logPath;
		obs_log(LOG_INFO, "Log path is %s", logPath);
	} else {
		context->logger.basedir = "";
		obs_log(LOG_INFO, "Log disabled");
	}

	if (context->gameplaySource) {
		obs_source_release(context->gameplaySource);
	}
	const char *gameplayName =
		obs_data_get_string(settings, "gameplay_source");
	obs_source_t *gameplaySource = obs_get_source_by_name(gameplayName);
	context->gameplaySource = gameplaySource;
}

extern "C" void screen_video_tick(void *data, float seconds)
{
	screen_context *context = static_cast<screen_context *>(data);
	uint64_t cur_time = os_gettime_ns();

	if (cur_time < context->next_tick + 1000 * 1000 * 100) {
		return;
	}
	context->next_tick = cur_time + 1000 * 1000 * 100;

	if (context->gameplay_bgra.empty()) {
		return;
	}

	cv::Mat gameplay_bgr, gameplay_hsv, gameplay_gray, gameplay_binary;
	cv::cvtColor(context->gameplay_bgra, gameplay_bgr, cv::COLOR_BGRA2BGR);
	cv::cvtColor(gameplay_bgr, gameplay_hsv, cv::COLOR_BGR2HSV);
	cv::cvtColor(context->gameplay_bgra, gameplay_gray,
		     cv::COLOR_BGRA2GRAY);
	cv::threshold(gameplay_gray, gameplay_binary, 200, 255,
		      cv::THRESH_BINARY);

	context->automaton(context->gameplay_bgra);

	UNUSED_PARAMETER(seconds);
}
