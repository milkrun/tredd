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



static const int min_width = 60;
static const int min_height = 360;
static const int min_x = 144 - 60 - 10;

static const int hour_width = 744;
static const int hour_height = 46;
static const int hour_y = 84 - 8;


static void deinit(void) {
	layer_remove_from_parent(bitmap_layer_get_layer(tread_min_1_layer));
	bitmap_layer_destroy(tread_min_1_layer);
	gbitmap_destroy(tread_min_1_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_1_layer));
	bitmap_layer_destroy(tread_hour_1_layer);
	gbitmap_destroy(tread_hour_1_image);
}

static int test_hours = 0;
static int test_hours_count = 0;

static int getHourX(int h) {
	int x = 10 - h * 62 ;
	return x;
}

static int last_hour = -1;

static void update_display(struct tm *current_time) {

  // update_hand_positions(); // TODO: Pass tick event
  
 int seconds = current_time->tm_min;
//  int seconds = current_time->tm_sec;
  
  int y = 84 - (6 * seconds);  
  
  
  // if y is greater than 0, tread 2 should be above tread 1
  // if y is less than 0, tread 2 should be above tread 1

  int y2 = 0;
  
  if (y > -10) {
      y2 = y - min_height;
  } else {
      y2 = y + min_height;
  }
  
  	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: y y2 %d %d", y, y2);

    
	layer_set_frame(bitmap_layer_get_layer(tread_min_1_layer), GRect(min_x, y, min_width, min_height));

	layer_set_frame(bitmap_layer_get_layer(tread_min_2_layer), GRect(min_x, y2, min_width, min_height));

    
  // int hours = seconds % 12;
  
	int hours = current_time->tm_hour;
	
//	int hours = current_time->tm_sec / 4;

// 	int hours = test_hours;

// 	if (test_hours_count++ > 2) {
// 		test_hours_count = 0;
// 		test_hours++;
// 	} else {
// 		return;
//   	}
  	
  	if (hours == last_hour) {
  		return;
  	}
  	last_hour = hours;
  	
	hours = hours % 12;
  
  	// hours = 0;  // test noon
  
	int x = 10 - hours * 62 ;
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: hours %d x %d", hours, x);

//   layer_set_frame((Layer*) (&tread_hour_1.layer.layer), GRect(x, hour_y, hour_width, hour_height));
  
  	GRect hour_rect = (GRect) {
		.origin = { .x = x, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	int x2 = getHourX(hours - 1);
  
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

static void init(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: init being called");

	window = window_create();
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


}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

