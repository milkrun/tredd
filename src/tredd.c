// copyright (c) 2014 Francis Stanbach Jr. all rights reserved.

#include "pebble.h"

static Window *window;

static GBitmap *background_image;
static BitmapLayer *background_layer;

// you need 2 minute treads to appear as a continuous tread

static GBitmap *tread_min_1_image;
static BitmapLayer *tread_min_1_layer;

static GBitmap *tread_min_2_image;
static BitmapLayer *tread_min_2_layer;

// 2 hour treads

static GBitmap *tread_hour_1_image;
static BitmapLayer *tread_hour_1_layer;

static GBitmap *tread_hour_2_image;
static BitmapLayer *tread_hour_2_layer;



static const int min_width = 60;
static const int min_height = 360;
static const int min_x = 144 - 60 - 10;

static const int hour_width = 744;
static const int hour_height = 40;
static const int hour_y = 84 - 20;


static void deinit(void) {
	layer_remove_from_parent(bitmap_layer_get_layer(tread_min_1_layer));
	bitmap_layer_destroy(tread_min_1_layer);
	gbitmap_destroy(tread_min_1_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_1_layer));
	bitmap_layer_destroy(tread_hour_1_layer);
	gbitmap_destroy(tread_hour_1_image);
}

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
//	int hours = current_time->tm_sec;
  
// 	if (hours > 12) {
// 		hours = hours - 12;
// 	}

	hours = hours % 12;
  
	int x = 5 - hours * 62 ;
  
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: hours %d x %d", hours, x);

//   layer_set_frame((Layer*) (&tread_hour_1.layer.layer), GRect(x, hour_y, hour_width, hour_height));
  
  	layer_set_frame(bitmap_layer_get_layer(tread_hour_1_layer), GRect(x, hour_y, hour_width, hour_height));

  
  
  
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

// 	tread_hour_2_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_22);
// 		
// 	tread_hour_2_layer = bitmap_layer_create(hour_frame);
// 	bitmap_layer_set_bitmap(tread_hour_2_layer, tread_hour_2_image);
// 	layer_add_child(window_layer, bitmap_layer_get_layer(tread_hour_2_layer));




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
	tread_min_2_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_60);
 
	tread_min_2_layer = bitmap_layer_create(min_frame);
	bitmap_layer_set_bitmap(tread_min_2_layer, tread_min_2_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_min_2_layer));



	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	update_display(tick_time);

//   tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);


}




// void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
//   (void)t;
// 
//   // update_hand_positions(); // TODO: Pass tick event
//   
//   int seconds = t->tick_time->tm_min;
//   
//   int y = 84 - (6 * seconds);  
//   
//   
//   // if y is greater than 0, tread 2 should be above tread 1
//   // if y is less than 0, tread 2 should be above tread 1
// 
//   int y2 = 0;
//   
//   if (y > -10) {
//       y2 = y - min_height;
//   } else {
//       y2 = y + min_height;
//   }
//   
//   layer_set_frame((Layer*) (&tread_min_1.layer.layer), GRect(min_x, y, min_width, min_height));
// 
//   layer_set_frame((Layer*) (&tread_min_2.layer.layer), GRect(min_x, y2, min_width, min_height));
//   
//   // int hours = seconds % 12;
//   
//   int hours = t->tick_time->tm_hour;
//   
//   if (clock_is_24h_style()) {
//     if (hours == 0) {
//       hours = 12;
//     } else if (hours > 12 {
//       hours = hours - 12;
//     }
//   }
// 
//   
//   int x = 10 - (hours - 1) * 62 ;
//   
//   layer_set_frame((Layer*) (&tread_hour_1.layer.layer), GRect(x, hour_y, hour_width, hour_height));
//   
//   
//   
//   
//   
// }

int main(void) {
  init();
  app_event_loop();
  deinit();
}

