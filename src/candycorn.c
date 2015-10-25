#include <pebble.h>

static Window *main_window;
static Layer *square1, *square2, *square3;
static TextLayer *time_layer, *date_layer, *day_layer;
static GFont time_font;

static char time_buffer[] = "00:00";
static char week_buffer[] = "WWW";
static char date_buffer[] = "DD";

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	if(clock_is_24h_style() == true) {
		//strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
		strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
		strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
	}

	strftime(date_buffer, sizeof("WWW"), "%a", tick_time);
	strftime(week_buffer, sizeof("DD"), "%e", tick_time);

	text_layer_set_text(time_layer, time_buffer);
	text_layer_set_text(date_layer, date_buffer); 
	text_layer_set_text(day_layer, week_buffer); 
}

static void square1_draw(Layer *layer, GContext *ctx) {
	GRect size = layer_get_bounds(square1);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, size, 0, GCornerNone);
}

static void square2_draw(Layer *layer, GContext *ctx) {
	GRect size = layer_get_bounds(square2);

	graphics_context_set_fill_color(ctx, GColorOrange);
	graphics_fill_rect(ctx, size, 0, GCornerNone);
}

static void square3_draw(Layer *layer, GContext *ctx) {
	GRect size = layer_get_bounds(square3);

	graphics_context_set_fill_color(ctx, GColorYellow);
	graphics_fill_rect(ctx, size, 0, GCornerNone);
}

static void main_window_load(Window *window) {
	GRect bounds = layer_get_bounds(window_get_root_layer(window)); // get size of window

	// create layers for bg
	square1 = layer_create(GRect(0, 0, bounds.size.w, (bounds.size.h / 3)));
	GRect square1_size = layer_get_bounds(square1);
	square2 = layer_create(GRect(0, (bounds.size.h / 3), bounds.size.w, (bounds.size.h / 3)));
	GRect square2_size = layer_get_bounds(square2);
	square3 = layer_create(GRect(0, ((bounds.size.h / 3) * 2), bounds.size.w, (bounds.size.h / 3)));
	GRect square3_size = layer_get_bounds(square3);

	// set draw funcs
	layer_set_update_proc(square1, square1_draw);
	layer_set_update_proc(square2, square2_draw);
	layer_set_update_proc(square3, square3_draw);

	// add bg children
	layer_add_child(window_get_root_layer(window), square1);
	layer_add_child(window_get_root_layer(window), square2);
	layer_add_child(window_get_root_layer(window), square3);


	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ABSENDER_48));

	date_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	text_layer_set_font(date_layer, time_font);
	text_layer_set_text_color(date_layer, GColorOrange);
	text_layer_set_text(date_layer, "   ");
	GSize date_size = text_layer_get_content_size(date_layer);
	#ifdef PBL_RECT
		layer_set_frame(text_layer_get_layer(date_layer), GRect(0, -3, bounds.size.w, bounds.size.h));
	#else
		layer_set_frame(text_layer_get_layer(date_layer), GRect(0, 0, bounds.size.w, bounds.size.h));
	#endif
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);

	time_layer = text_layer_create(GRect(0, 50, bounds.size.w, 50));
	text_layer_set_font(time_layer, time_font);
	text_layer_set_text_color(time_layer, GColorYellow);
	text_layer_set_text(time_layer, "     ");
	GSize time_size = text_layer_get_content_size(time_layer);
	#ifdef PBL_ROUND
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, ((square2_size.size.h / 2) + (time_size.h / 2)) + 5, bounds.size.w, bounds.size.h));
	#else
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, ((square2_size.size.h / 2) + (time_size.h / 2)), bounds.size.w, bounds.size.h)); // ((square2_size.size.h / 2) + (time_size.h / 2))
	#endif
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);

	day_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	text_layer_set_font(day_layer, time_font);
	text_layer_set_text_color(day_layer, GColorBlack);
	text_layer_set_text(day_layer, "  ");
	GSize day_size = text_layer_get_content_size(day_layer);
	layer_set_frame(text_layer_get_layer(day_layer), GRect(0, ((bounds.size.h / 3) * 2) - 3, bounds.size.w, bounds.size.h));
	text_layer_set_background_color(day_layer, GColorClear);
	text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);


	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(day_layer));

	update_time();
}

static void main_window_unload(Window *window) {
	layer_destroy(square1);
	layer_destroy(square2);
	layer_destroy(square3);

	text_layer_destroy(date_layer);
	text_layer_destroy(time_layer);
	text_layer_destroy(day_layer);

	fonts_unload_custom_font(time_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}


static void init() {
	main_window = window_create();

	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(main_window, true);
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	window_destroy(main_window);
	tick_timer_service_unsubscribe();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}