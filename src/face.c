#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;

static void main_window_load(Window *window) {

  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Load large font
  GFont large_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LECO_50));

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 5, bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  //text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_font(s_time_layer, large_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Add to window
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
