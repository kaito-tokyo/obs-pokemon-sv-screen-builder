#include <obs-module.h>
#include <util/platform.h>

struct screen_context
{
	obs_source_t *source;
};


static void screen_main_render_callback(void *data, uint32_t cx, uint32_t cy)
{
	screen_context *context = reinterpret_cast<screen_context*>(data);

	uint32_t pixels[20 * 20] = {};
	uint64_t cur_time = os_gettime_ns();
	struct obs_source_frame frame = {
		.data = {[0] = (uint8_t *)pixels},
		.linesize = {[0] = 20 * 4},
		.width = 20,
		.height = 20,
		.format = VIDEO_FORMAT_BGRA,
		.timestamp = cur_time,
	};

	obs_source_output_video(context->source, &frame);

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
	screen_context *context = reinterpret_cast<screen_context*>(bzalloc(sizeof(screen_context)));
	context->source = source;

	obs_add_main_render_callback(screen_main_render_callback, context);

	UNUSED_PARAMETER(settings);
	return context;
}

static void screen_destroy(void *data)
{
	screen_context *context = reinterpret_cast<screen_context*>(data);

	if (context) {
		obs_remove_main_render_callback(screen_main_render_callback, context);
		bfree(context);
	}
}

struct obs_source_info screen_info = {
	.id = "obs-pokemon-sv-screen-builder",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO,
	.get_name = screen_get_name,
	.create = screen_create,
	.destroy = screen_destroy,
	// .video_render = screen_video_render,
	// .video_tick = filter_video_tick,
	// .get_properties = filter_properties,
	// .get_defaults = filter_defaults,
};