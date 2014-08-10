// copyright (c) 2014 Francis Stanbach Jr. all rights reserved.

#include "pebble.h"

static Window *window;

static GBitmap *background_image;
static BitmapLayer *background_layer;

// you need 2 minute treads to appear as a continuous tread

static GBitmap *tread_min_1_image;
static BitmapLayer *tread_min_1_layer;

static BitmapLayer *tread_min_2_layer;

// 2 hour treads

static GBitmap *tread_hour_1_image;
static BitmapLayer *tread_hour_1_layer;

// static GBitmap *tread_hour_2_image;
static BitmapLayer *tread_hour_2_layer;

static PropertyAnimation *hour_animation_1;
static PropertyAnimation *hour_animation_2;

// overlay

static GBitmap *white_image;
static GBitmap *black_image;
static BitmapLayer *white_image_layer;
static BitmapLayer *black_image_layer;

//

static int date_timer = 0;

static const int TIME = 0;
static const int DATE = 1;
static const int CHRONO = 2;


static int mode = 0; // TIME

static int chrono_minutes = 0;
static int chrono_seconds = 0;

static int chrono_running = 0;	// stopped 0 running 1


static const int min_width = 60;
static const int min_height = 360;
static const int min_x = 144 - 60 - 10;

static const int hour_width = 744;
static const int hour_height = 46;
static const int hour_y = 84 - 8;

// static int test_hours = 0;
// static int test_hours_count = 0;

static int getHourX(int h) {
	int x = 10 - h * 62 ;
	return x;
}

static int last_hour = -1;


static void show_this(int hour, int minutes);



// I want to vibe the watch 5 seconds before the hour
// between 9am and 9pm

static void hour_vibe(struct tm *current_time) {

	if (current_time->tm_hour < 7) 
		return;

	if (current_time->tm_hour > 20) 
		return;

	if (current_time->tm_min != 59)
		return;

	if (current_time->tm_sec != 55)
		return;
		
	vibes_short_pulse();

}



static void update_display(struct tm *current_time) {

	if (mode == TIME) {
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: tick --- %d %d %d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
  	} else if (mode == DATE) {
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: date ---");
  	} else if (mode == CHRONO) {
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: chrono ---");
  	} 

	if (mode == DATE) {
// 		date_timer--;
//   		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: date_timer %d", date_timer);
// 		if (date_timer == 0) {
//   			APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: switch to time mode");
// 		}
// 		
// 		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: date mode %d %d ", current_time->tm_mon, current_time->tm_mday);
		
		
		int month = current_time->tm_mon + 1;
		if (month == 12) {
			month = 0;
		}
		
		show_this(month, current_time->tm_mday);

	} else if (mode == TIME) { 
	
		// show time
	
		show_this(current_time->tm_hour, current_time->tm_min);
	
		hour_vibe(current_time);
	
	} else if (mode == CHRONO) { 
	
		// show time
	
		show_this(chrono_minutes, chrono_seconds);
	
		if (chrono_running == 1) {
			chrono_seconds++;
			if (chrono_seconds > 59) {
				chrono_seconds = 0;
				chrono_minutes++;
			}
		}
	}
	
}
  
static void show_this(int hour, int minutes) {
 
  	int y = 84 - (6 * minutes);  
  
  
  // if y is greater than 0, tread 2 should be above tread 1
  // if y is less than 0, tread 2 should be above tread 1

  int y2 = 0;
  
  if (y > -10) {
      y2 = y - min_height;
  } else {
      y2 = y + min_height;
  }
  
//   	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: y y2 %d %d", y, y2);

    
	layer_set_frame(bitmap_layer_get_layer(tread_min_1_layer), GRect(min_x, y, min_width, min_height));

	layer_set_frame(bitmap_layer_get_layer(tread_min_2_layer), GRect(min_x, y2, min_width, min_height));

  
//	int hour = current_time->tm_hour;
	
//	int hour = current_time->tm_sec / 4;

// 	int hour = test_hour;

// 	if (test_hour_count++ > 2) {
// 		test_hour_count = 0;
// 		test_hour++;
// 	} else {
// 		return;
//   	}
  	
  	if (hour == last_hour) {
  		return;
  	}
  	last_hour = hour;
  	
	hour = hour % 12;
  
  	// hour = 0;  // test noon
  
	int x = 10 - hour * 62 ;
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: hour %d x %d", hour, x);

//   layer_set_frame((Layer*) (&tread_hour_1.layer.layer), GRect(x, hour_y, hour_width, hour_height));
  
  	GRect hour_rect = (GRect) {
		.origin = { .x = x, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	int x2 = getHourX(hour - 1);
  
  	GRect hour_start_rect = (GRect) {
		.origin = { .x = x2, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};


	if (hour_animation_1 != NULL) {
   		animation_destroy((Animation*) hour_animation_1);
   	}
  
    hour_animation_1 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_hour_1_layer), &hour_start_rect, &hour_rect);
  	animation_set_duration((Animation*) hour_animation_1, 1000);
//  animation_set_curve((Animation*) hour_animation_1, AnimationCurveEaseOut);
    animation_schedule((Animation*) hour_animation_1);

  
//  	layer_set_frame(bitmap_layer_get_layer(tread_hour_1_layer), GRect(x, hour_y, hour_width, hour_height));

  
	int xx;
  
  	if (x < -62) {
		xx = x + tread_hour_1_image->bounds.size.w;
	} else {
		xx = x - tread_hour_1_image->bounds.size.w;
	}
  
  	if (hour_animation_2 != NULL) {
   		animation_destroy((Animation*) hour_animation_2);
   	}

  	GRect hour_rect_2 = (GRect) {
		.origin = { .x = xx, .y = hour_y},
		.size = tread_hour_1_image->bounds.size
	};

  	int xx2 = xx + 62;
  
  	GRect hour_start_rect_2 = (GRect) {
		.origin = { .x = xx2, .y = hour_y},
		.size = tread_hour_1_image->bounds.size
	};

  	// layer_set_frame(bitmap_layer_get_layer(tread_hour_2_layer), GRect(xx2, hour_y + 40, hour_width, hour_height));

    hour_animation_2 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_hour_2_layer), &hour_start_rect_2, &hour_rect_2);
  	animation_set_duration((Animation*) hour_animation_2, 1000);
//  animation_set_curve((Animation*) hour_animation_2, AnimationCurveEaseOut);
    animation_schedule((Animation*) hour_animation_2);

  
  
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_display(tick_time);
}

// void accel_tap_handler(AccelAxisType axis, int32_t direction) {
//   // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
//   // Direction is 1 or -1
//   
//   	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: being tapped");
//   	
//   	// switching to different mode
//   	// create a timer to switch back to normal mode
// 
// 	date_timer = 10;
// }


void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: select button");

	if (mode == TIME) {
  		mode = DATE;
  	} else if (mode == DATE) {
  		mode = CHRONO;
  	} else if (mode == CHRONO) {
  		mode = TIME;
  	} 

}

void chrono_start_stop_click_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: chrono start stop");

	if (chrono_running == 0) {
  		chrono_running = 1;
  	} else  {
  		chrono_running = 0;
  	} 

}

void chrono_reset_click_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: chrono reset");

	chrono_minutes = 0;
	chrono_seconds = 0;

}

void config_provider(Window *window) {
 // single click / repeat-on-hold config:
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, chrono_start_stop_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, chrono_reset_click_handler);

//   window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
//   // multi click config:
//   window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
//   // long click config:
//   window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

static void init(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: init being called");

	window = window_create();
	window_set_fullscreen(window, true);
	if (window == NULL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
		return;
	}
	window_stack_push(window, true /* Animated */);
    Layer *window_layer = window_get_root_layer(window);
    
    // load background
	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	background_layer = bitmap_layer_create(layer_get_frame(window_layer));
	bitmap_layer_set_bitmap(background_layer, background_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

	// load first copy of hour tread
	tread_hour_1_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_12);
	
	GRect hour_frame = (GRect) {
		.origin = { .x = 42, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};
	
	tread_hour_1_layer = bitmap_layer_create(hour_frame);
	bitmap_layer_set_bitmap(tread_hour_1_layer, tread_hour_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_hour_1_layer));

	hour_animation_1 = NULL;

	// second hour tread

// 	tread_hour_2_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_12);
		
	GRect hour_frame_2 = (GRect) {
		.origin = { .x = 42, .y = hour_y},
		.size = tread_hour_1_image->bounds.size
	};
	
	tread_hour_2_layer = bitmap_layer_create(hour_frame_2);
	bitmap_layer_set_bitmap(tread_hour_2_layer, tread_hour_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_hour_2_layer));

	hour_animation_2 = NULL;

	// load first copy of minute tread
	tread_min_1_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_60);
 
	GRect min_frame = (GRect) {
		.origin = { .x = min_x, .y = 43 },
		.size = tread_min_1_image->bounds.size
	};

	tread_min_1_layer = bitmap_layer_create(min_frame);
	bitmap_layer_set_bitmap(tread_min_1_layer, tread_min_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_min_1_layer));

	// load second copy of minute tread
// 	tread_min_2_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_60);
 
	tread_min_2_layer = bitmap_layer_create(min_frame);
	bitmap_layer_set_bitmap(tread_min_2_layer, tread_min_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_min_2_layer));

	// add the transparent overlay
	
	white_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_WHITE);
	black_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_BLACK);

  	GRect bounds = layer_get_bounds(window_layer);
	
	const GPoint center = grect_center_point(&bounds);
	GRect image_frame = (GRect) { .origin = center, .size = white_image->bounds.size };
	image_frame.origin.x -= white_image->bounds.size.w/2;
	image_frame.origin.y -= white_image->bounds.size.h/2;

	// Use GCompOpOr to display the white portions of the image
	white_image_layer = bitmap_layer_create(image_frame);
	bitmap_layer_set_bitmap(white_image_layer, white_image);
	bitmap_layer_set_compositing_mode(white_image_layer, GCompOpOr);
	layer_add_child(window_layer, bitmap_layer_get_layer(white_image_layer));

	// Use GCompOpClear to display the black portions of the image
	black_image_layer = bitmap_layer_create(image_frame);
	bitmap_layer_set_bitmap(black_image_layer, black_image);
	bitmap_layer_set_compositing_mode(black_image_layer, GCompOpClear);
	layer_add_child(window_layer, bitmap_layer_get_layer(black_image_layer));


	// update time after init to avoid the unsightly wait


	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	update_display(tick_time);

//   tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);

// 	accel_tap_service_subscribe(accel_tap_handler);

	window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

}

static void deinit(void) {

	layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
	bitmap_layer_destroy(background_layer);
	gbitmap_destroy(background_image);

	layer_remove_from_parent(bitmap_layer_get_layer(tread_min_1_layer));
	bitmap_layer_destroy(tread_min_1_layer);
	layer_remove_from_parent(bitmap_layer_get_layer(tread_min_2_layer));
	bitmap_layer_destroy(tread_min_2_layer);
	gbitmap_destroy(tread_min_1_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_1_layer));
	bitmap_layer_destroy(tread_hour_1_layer);
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_2_layer));
	bitmap_layer_destroy(tread_hour_2_layer);
	gbitmap_destroy(tread_hour_1_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(white_image_layer));
	bitmap_layer_destroy(white_image_layer);
	gbitmap_destroy(white_image);

	layer_remove_from_parent(bitmap_layer_get_layer(black_image_layer));
	bitmap_layer_destroy(black_image_layer);
	gbitmap_destroy(black_image);

	property_animation_destroy(hour_animation_1);
	property_animation_destroy(hour_animation_2);
  
	window_destroy(window);

//   	accel_tap_service_unsubscribe();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

