#include <obs-module.h>
#include <util/platform.h>

struct screen_context
{
	obs_source_t *source;
	uint32_t width;
	uint32_t height;
	uint32_t *pixels_bgra;
};


static inline void fill_texture(uint32_t *pixels)
{
	size_t x, y;

	for (y = 0; y < 1080; y++) {
		for (x = 0; x < 1920; x++) {
			uint32_t pixel = 0;
			pixel |= (255 % 256);
			pixel |= (255 % 256) << 8;
			pixel |= (0 % 256) << 16;
		    pixel |= 0xFF << 24;
			pixels[y * 1920 + x] = pixel;
		}
	}
}

static void screen_main_render_callback(void *data, uint32_t cx, uint32_t cy)
{
	screen_context *context = reinterpret_cast<screen_context*>(data);

	fill_texture(context->pixels_bgra);
	uint64_t cur_time = os_gettime_ns();
	struct obs_source_frame frame = {
		.data = {[0] = reinterpret_cast<uint8_t*>(context->pixels_bgra)},
		.linesize = {[0] = context->width * 4},
		.width = context->width,
		.height = context->height,
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
	context->width = 1920;
	context->height = 1080;
	context->pixels_bgra = reinterpret_cast<uint32_t*>(bzalloc(context->width * context->height));

	obs_add_main_render_callback(screen_main_render_callback, context);

	UNUSED_PARAMETER(settings);
	return context;
}

static void screen_destroy(void *data)
{
	screen_context *context = reinterpret_cast<screen_context*>(data);

	if (context) {
		obs_remove_main_render_callback(screen_main_render_callback, context);
		if (context->pixels_bgra) {
			bfree(context->pixels_bgra);
		}
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