#include <array>
#include <inttypes.h>

#include <opencv2/opencv.hpp>

#include <obs-module.h>

#include "EntityCropper.h"
#include "SceneDetector.h"
#include "SelectionRecognizer.h"

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

extern "C" EXPORT uint64_t os_gettime_ns(void);

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

const std::array<int, 2> opponent_col_range{1239, 1337};
const std::vector<std::array<int, 2>> opponent_row_range{{{228, 326},
							  {330, 428},
							  {432, 530},
							  {534, 632},
							  {636, 734},
							  {738, 836}}};

const std::array<int, 2> myPokemonColRange{182, 711};
const std::vector<std::array<int, 2>> myPokemonRowRange{{{147, 254},
							 {263, 371},
							 {379, 486},
							 {496, 602},
							 {612, 718},
							 {727, 834}}};

const std::array<int, 2> selectionOrderColRange{795, 827};
const std::vector<std::array<int, 2>> selectionOrderRowRange{{{154, 186},
							      {271, 303},
							      {388, 420},
							      {503, 535},
							      {619, 651},
							      {735, 767}}};

constexpr int N_POKEMONS = 6;

struct screen_config {
	uint32_t width;
	uint32_t height;

	const bool skipMySelection;
	const std::vector<int> myPokemonPlacementX;
	const int myPokemonPlacementY;
	const cv::Size myPokemonSize;

	const bool skipOpponentTeam;
	const int opponentPokemonPlacementX;
	const std::vector<int> opponentPokemonPlacementY;
	const cv::Size opponentPokemonSize;
};

const screen_config defaultScreenConfig{
	.width = 1920,
	.height = 1080,
	.skipMySelection = false,
	.myPokemonPlacementX = {0, 391, 782, 0, 0, 0},
	.myPokemonPlacementY = 1000,
	.myPokemonSize = {391, 80},
	.skipOpponentTeam = false,
	.opponentPokemonPlacementX = 1776,
	.opponentPokemonPlacementY = {0, 144, 288, 432, 576, 720},
	.opponentPokemonSize = {144, 144},
};

const screen_config mySelectionScreenConfig{
	.width = 1173,
	.height = 80,
	.skipMySelection = false,
	.myPokemonPlacementX = {0, 391, 782, 0, 0, 0},
	.myPokemonPlacementY = 0,
	.myPokemonSize = {391, 80},
	.skipOpponentTeam = true,
	.opponentPokemonPlacementX = 0,
	.opponentPokemonPlacementY = {0, 144, 288, 432, 576, 720},
	.opponentPokemonSize = {144, 144},
};

const screen_config opponentTeamScreenConfig{
	.width = 144,
	.height = 864,
	.skipMySelection = true,
	.myPokemonPlacementX = {0, 391, 782, 0, 0, 0},
	.myPokemonPlacementY = 0,
	.myPokemonSize = {391, 80},
	.skipOpponentTeam = false,
	.opponentPokemonPlacementX = 0,
	.opponentPokemonPlacementY = {0, 144, 288, 432, 576, 720},
	.opponentPokemonSize = {144, 144},
};

struct screen_context {
	obs_data_t *settings = nullptr;
	obs_source_t *source = nullptr;

	gs_texrender_t *texrender = nullptr;
	gs_stagesurf_t *stagesurface = nullptr;
	cv::Mat gameplay_bgra;

	uint64_t next_tick = 0;
	SceneDetector sceneDetector;
	cv::Mat screen_bgra;

	screen_state state = STATE_UNKNOWN;
	uint64_t last_state_change_ns = 0;
	int my_selection_order_map[N_POKEMONS]{};
	SceneDetector::Scene prev_scene;
	uint64_t match_start_ns = 0;
	uint64_t last_elapsed_seconds = 0;
	uint64_t match_end_ns = 0;

	EntityCropper opponentPokemonCropper;
	EntityCropper myPokemonCropper;
	EntityCropper selectionOrderCropper;
	SelectionRecognizer selectionRecognizer;

	cv::Mat myPokemonsBGRA[N_POKEMONS];

	screen_config config;

	screen_context(const screen_config &_config)
		: sceneDetector(classifier_lobby_my_select,
				classifier_lobby_opponent_select,
				classifier_black_transition),
		  opponentPokemonCropper(opponent_col_range,
					 opponent_row_range),
		  myPokemonCropper(myPokemonColRange, myPokemonRowRange),
		  selectionOrderCropper(selectionOrderColRange,
					selectionOrderRowRange),
		  config(_config)
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

static const char *screen_my_selection_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("PokemonSVScreenBuilderMySelection");
}

static const char *screen_opponent_team_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("PokemonSVScreenBuilderOpponentTeam");
}

static void *screen_create(obs_data_t *settings, obs_source_t *source)
{
	void *rawContext = bmalloc(sizeof(screen_context));
	screen_context *context = new (rawContext)
		screen_context(defaultScreenConfig);
	context->settings = settings;
	context->source = source;

	context->texrender = gs_texrender_create(GS_BGRA, GS_ZS_NONE);

	obs_add_main_render_callback(screen_main_render_callback, context);

	UNUSED_PARAMETER(settings);
	return context;
}

static void *screen_my_selection_create(obs_data_t *settings,
					obs_source_t *source)
{
	void *rawContext = bmalloc(sizeof(screen_context));
	screen_context *context = new (rawContext)
		screen_context(mySelectionScreenConfig);
	context->settings = settings;
	context->source = source;

	context->texrender = gs_texrender_create(GS_BGRA, GS_ZS_NONE);

	obs_add_main_render_callback(screen_main_render_callback, context);

	UNUSED_PARAMETER(settings);
	return context;
}

static void *screen_opponent_team_create(obs_data_t *settings,
					 obs_source_t *source)
{
	void *rawContext = bmalloc(sizeof(screen_context));
	screen_context *context = new (rawContext)
		screen_context(opponentTeamScreenConfig);
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

static uint32_t screen_get_width(void *data)
{
	screen_context *context = reinterpret_cast<screen_context *>(data);
	return context->config.width;
}

static uint32_t screen_get_height(void *data)
{
	screen_context *context = reinterpret_cast<screen_context *>(data);
	return context->config.height;
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
		props, "gameplay_source", obs_module_text("GamePlaySource"),
		OBS_COMBO_TYPE_EDITABLE, OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_all_sources_to_list, prop_gameplay_source);

	obs_property_t *prop_timer = obs_properties_add_list(
		props, "timer_source", obs_module_text("TimerSource"),
		OBS_COMBO_TYPE_EDITABLE, OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(add_text_sources_to_list, prop_timer);

	return props;
}

static void drawOpponentPokemons(screen_context *context)
{
	context->opponentPokemonCropper.crop(context->gameplay_bgra);
	context->opponentPokemonCropper.generateMask();
	for (int i = 0; i < N_POKEMONS; i++) {
		auto x = context->config.opponentPokemonPlacementX;
		auto y = context->config.opponentPokemonPlacementY[i];
		auto &pokemonBGRA =
			context->opponentPokemonCropper.imagesBGRA[i];
		cv::Mat resizedBGRA;
		cv::resize(pokemonBGRA, resizedBGRA,
			   context->config.opponentPokemonSize);
		resizedBGRA.copyTo(
			context->screen_bgra.rowRange(y, y + resizedBGRA.rows)
				.colRange(x, x + resizedBGRA.cols));
	}
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
		int pokemon = context->my_selection_order_map[i];
		if (pokemon == 0)
			continue;

		auto pokemonBGRA = context->myPokemonsBGRA[pokemon - 1];
		if (pokemonBGRA.empty())
			continue;
		cv::Mat resizedBGRA;
		cv::resize(pokemonBGRA, resizedBGRA,
			   context->config.myPokemonSize);

		auto x = context->config.myPokemonPlacementX[i];
		auto y = context->config.myPokemonPlacementY;
		resizedBGRA.copyTo(
			context->screen_bgra.rowRange(y, y + resizedBGRA.rows)
				.colRange(x, x + resizedBGRA.cols));
	}
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

	if (context->screen_bgra.rows != context->gameplay_bgra.rows ||
	    context->screen_bgra.cols != context->gameplay_bgra.cols) {
		context->screen_bgra = cv::Mat(context->config.width,
					       context->config.height, CV_8UC4,
					       cv::Scalar(0));
	}

	if (context->state == STATE_UNKNOWN) {
		cv::Mat screenTextBinary =
			context->sceneDetector.generateTextBinaryScreen(
				context->gameplay_bgra);

		if (context->sceneDetector.isOpponentRankShown(
			    screenTextBinary)) {
			blog(LOG_INFO, "Rank shown!");
		};
		if (scene == SceneDetector::SCENE_SELECT_POKEMON) {
			context->state = STATE_ENTERING_SELECT_POKEMON;
			context->last_state_change_ns = os_gettime_ns();
			if (!context->gameplay_bgra.empty()) {
				context->screen_bgra =
					cv::Mat(context->gameplay_bgra.rows,
						context->gameplay_bgra.cols,
						CV_8UC4, cv::Scalar(0));
			}
			blog(LOG_INFO, "State: UNKNOWN to ENTERING_SELECT");
		}
	} else if (context->state == STATE_ENTERING_SELECT_POKEMON) {
		const uint64_t now = os_gettime_ns();
		if (now - context->last_state_change_ns > 1000000000) {
			if (!context->config.skipOpponentTeam) {
				drawOpponentPokemons(context);
			}
			for (int i = 0; i < N_POKEMONS; i++) {
				context->my_selection_order_map[i] = 0;
			}
			context->state = STATE_SELECT_POKEMON;
			blog(LOG_INFO,
			     "State: ENTERING_SELECT_POKEMON to SELECT_POKEMON");
		}
	} else if (context->state == STATE_SELECT_POKEMON) {
		if (detectSelectionOrderChange(context) &&
		    !context->config.skipMySelection) {
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

	if (!context->screen_bgra.empty()) {
		struct obs_source_frame frame = {
			.width = static_cast<uint32_t>(
				context->screen_bgra.cols),
			.height = static_cast<uint32_t>(
				context->screen_bgra.rows),
			.timestamp = cur_time,
			.format = VIDEO_FORMAT_BGRA,
		};
		frame.data[0] = context->screen_bgra.data;
		frame.linesize[0] = context->screen_bgra.cols * 4;

		obs_source_output_video(context->source, &frame);
	}

	UNUSED_PARAMETER(seconds);
}

struct obs_source_info screen_info = {
	.id = "obs-pokemon-sv-screen-builder",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO,
	.get_name = screen_get_name,
	.create = screen_create,
	.destroy = screen_destroy,
	.get_width = screen_get_width,
	.get_height = screen_get_height,
	.get_defaults = screen_defaults,
	.get_properties = screen_properties,
	.video_tick = screen_video_tick,
};

struct obs_source_info screen_my_selection_info = {
	.id = "obs-pokemon-sv-screen-builder-my-selection",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO,
	.get_name = screen_my_selection_get_name,
	.create = screen_my_selection_create,
	.destroy = screen_destroy,
	.get_width = screen_get_width,
	.get_height = screen_get_height,
	.get_defaults = screen_defaults,
	.get_properties = screen_properties,
	.video_tick = screen_video_tick,
};

struct obs_source_info screen_opponent_team_info = {
	.id = "obs-pokemon-sv-screen-builder-opponent-team",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO,
	.get_name = screen_opponent_team_get_name,
	.create = screen_opponent_team_create,
	.destroy = screen_destroy,
	.get_width = screen_get_width,
	.get_height = screen_get_height,
	.get_defaults = screen_defaults,
	.get_properties = screen_properties,
	.video_tick = screen_video_tick,
};
