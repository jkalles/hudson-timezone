/** \file
 * Three time zone clock;
 *
 * When it is day, the text and background are white.
 * When it is night, they are black.
 *
 * Rather than use text layers, it draws the entire frame once per minute.
 *
 * Inspired by a design on RichardG's site that I can't find again to
 * credit the designer.
 */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "pebble_th.h"

#define UUID { 0xE6, 0x87, 0x18, 0x07, 0x0E, 0x8A, 0x4F, 0xC2, 0xB6, 0xFF, 0x0E, 0x66, 0xA9, 0xC1, 0x7E, 0xA7 }

PBL_APP_INFO(
	UUID,
	"Timezones",
	"hudson",
	1, 0, // Version
	RESOURCE_ID_IMAGE_MENU_ICON,
	APP_INFO_WATCH_FACE
);


typedef struct
{
	const char * name;
	int offset;
	Layer layer;
} timezone_t;


// Local timezone GMT offset
static const int gmt_offset = -5 * 60;

#define NUM_TIMEZONES 3
#define LAYER_HEIGHT (PEBBLE_SCREEN_HEIGHT / NUM_TIMEZONES)

static timezone_t timezones[NUM_TIMEZONES] =
{
	{ .name = "New York", .offset = -5 * 60 },
	{ .name = "London", .offset = +0 * 60 },
	{ .name = "India", .offset = +(5 * 60 + 30) },
};


static Window window;
static PblTm now;
static GFont font_thin;
static GFont font_thick;


static void
timezone_layer_update(
        Layer * const me,
        GContext * ctx
)
{
	const timezone_t * const tz = container_of(me, timezone_t, layer);

	const int orig_hour = now.tm_hour;
	const int orig_min = now.tm_min;

	now.tm_min += (tz->offset - gmt_offset) % 60;
	if (now.tm_min > 60)
	{
		now.tm_hour++;
		now.tm_min -= 60;
	} else
	if (now.tm_min < 0)
	{
		now.tm_hour--;
		now.tm_min += 60;
	}

	now.tm_hour += (tz->offset - gmt_offset) / 60;
	if (now.tm_hour > 24)
		now.tm_hour -= 24;
	if (now.tm_hour < 0)
		now.tm_hour += 24;

	char buf[32];
	string_format_time(
		buf,
		sizeof(buf),
		"%H:%M",
		&now
	);


	const int night_time = (now.tm_hour > 18 || now.tm_hour < 6);
	now.tm_hour = orig_hour;
	now.tm_min = orig_min;

	const int w = me->bounds.size.w;
	const int h = me->bounds.size.h;
		
	// it is night there, draw in black video
	graphics_context_set_fill_color(ctx, night_time ? GColorBlack : GColorWhite);
	graphics_context_set_text_color(ctx, !night_time ? GColorBlack : GColorWhite);
	graphics_fill_rect(ctx, GRect(0, 0, w, h), 0, 0);

	graphics_text_draw(ctx,
		tz->name,
		font_thin,
		GRect(0, 0, w, h/3),
		GTextOverflowModeTrailingEllipsis,
		GTextAlignmentCenter,
		NULL
	);

	graphics_text_draw(ctx,
		buf,
		font_thick,
		GRect(0, h/3, w, 2*h/3),
		GTextOverflowModeTrailingEllipsis,
		GTextAlignmentCenter,
		NULL
	);
}


/** Called once per minute */
static void
handle_tick(
	AppContextRef ctx,
	PebbleTickEvent * const event
)
{
	(void) ctx;

	now = *event->tick_time;

	for (int i = 0 ; i < NUM_TIMEZONES ; i++)
		layer_mark_dirty(&timezones[i].layer);
}


static void
handle_init(
	AppContextRef ctx
)
{
	(void) ctx;
	get_time(&now);

	window_init(&window, "Main");
	window_stack_push(&window, true);
	window_set_background_color(&window, GColorBlack);

	resource_init_current_app(&RESOURCES);

	font_thin = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_16));
	font_thick = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_BLACK_30));

	for (int i = 0 ; i < NUM_TIMEZONES ; i++)
	{
		timezone_t * const tz = &timezones[i];
		layer_init(
			&tz->layer,
			GRect(0, i * LAYER_HEIGHT, PEBBLE_SCREEN_WIDTH, LAYER_HEIGHT)
		);

		tz->layer.update_proc = timezone_layer_update;
		layer_add_child(&window.layer, &tz->layer);
		layer_mark_dirty(&tz->layer);
	}
}


static void
handle_deinit(
	AppContextRef ctx
)
{
	(void) ctx;

	fonts_unload_custom_font(font_thin);
	fonts_unload_custom_font(font_thick);
}


void
pbl_main(
	void * const params
)
{
	PebbleAppHandlers handlers = {
		.init_handler	= &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info	= {
			.tick_handler = &handle_tick,
			.tick_units = MINUTE_UNIT,
		},
	};

	app_event_loop(params, &handlers);
}
