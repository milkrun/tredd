// copyright (c) 2014 Francis Stanbach Jr. all rights reserved.

#include "pebble.h"

static Window *window;

// BmpContainer tread_min_1;
// BmpContainer tread_min_2;
// BmpContainer tread_hour_1;
// BmpContainer tread_hour_2;

// BmpContainer tread_min_1;
// BmpContainer tread_min_2;
// BmpContainer tread_hour_1;
// BmpContainer tread_hour_2;

static GBitmap *background_image;
static BitmapLayer *background_layer;

static GBitmap *tread_min_1_image;
static BitmapLayer *tread_min_1_layer;

static GBitmap *tread_hour_1_image;
static BitmapLayer *tread_hour_1_layer;

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

// void handle_init(AppContextRef ctx) {
//   (void)ctx;
// 
//   window_init(&window, "Tread");
//   window_stack_push(&window, true /* Animated */);
//   window_set_background_color(&window, GColorBlack);
// 
//   resource_init_current_app(&APP_RESOURCES);
//   
//   bmp_init_container(RESOURCE_ID_IMAGE_TREAD_60, &tread_min_1);
//   bmp_init_container(RESOURCE_ID_IMAGE_TREAD_60, &tread_min_2);
//   bmp_init_container(RESOURCE_ID_IMAGE_TREAD_12, &tread_hour_1);
//   // bmp_init_container(RESOURCE_ID_IMAGE_TREAD_12, &tread_hour_2);
//   
//   tread_min_1.layer.layer.frame.origin.x = min_x;
//   tread_min_1.layer.layer.frame.origin.y = 10;
//   tread_min_2.layer.layer.frame.origin.x = min_x;
//   tread_min_2.layer.layer.frame.origin.y = 10;
//   
//   tread_hour_1.layer.layer.frame.origin.x = 10;
//   tread_hour_1.layer.layer.frame.origin.y = hour_y;
//   
//   // tread_hour_2.layer.layer.frame.origin.x = 10;
//   // tread_hour_2.layer.layer.frame.origin.y = hour_y;
// 
//   // layer_add_child(&window.layer, &tread_hour_2.layer.layer);
//   
//   layer_add_child(&window.layer, &tread_hour_1.layer.layer);
// 
// 
//   layer_add_child(&window.layer, &tread_min_1.layer.layer);
//   layer_add_child(&window.layer, &tread_min_2.layer.layer);
//   
// 
// 
// }

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;

  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect frame = (GRect) {
    .origin = origin,
    .size = (*bmp_image)->bounds.size
  };
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

  if (old_image != NULL) {
  	gbitmap_destroy(old_image);
  }
}

static void update_display(struct tm *current_time) {

  // update_hand_positions(); // TODO: Pass tick event
  
 // int seconds = current_time->tm_min;
  int seconds = current_time->tm_sec;
  
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

  
//    layer_set_frame(bitmap_layer_get_layer(tread_min_1_layer), GRect(min_x -100, y, min_width + 100, min_height));
    layer_set_frame(bitmap_layer_get_layer(tread_min_1_layer), GRect(min_x -60, y, 100, min_height));

  
//   layer_set_frame((Layer*) (&tread_min_1.layer.layer), GRect(min_x, y, min_width, min_height));

//   layer_set_frame((Layer*) (&tread_min_2.layer.layer), GRect(min_x, y2, min_width, min_height));
  
  // int hours = seconds % 12;
  
//   int hours = t->tick_time->tm_hour;
//   
// 	if (hours > 12 {
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
  
  
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_display(tick_time);
}


static void init(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: init being called");

// 	memset(&time_digits_layers, 0, sizeof(time_digits_layers));
// 	memset(&time_digits_images, 0, sizeof(time_digits_images));
// 	memset(&date_digits_layers, 0, sizeof(date_digits_layers));
// 	memset(&date_digits_images, 0, sizeof(date_digits_images));

	window = window_create();
	if (window == NULL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
		return;
	}
	window_stack_push(window, true /* Animated */);
    Layer *window_layer = window_get_root_layer(window);
      
      background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
      background_layer = bitmap_layer_create(layer_get_frame(window_layer));
      bitmap_layer_set_bitmap(background_layer, background_image);
      layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

	tread_hour_1_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_12);
	tread_hour_1_layer = bitmap_layer_create(layer_get_frame(window_layer));
	bitmap_layer_set_bitmap(tread_hour_1_layer, tread_hour_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_hour_1_layer));

	tread_min_1_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TREAD_60);
	tread_min_1_layer = bitmap_layer_create(layer_get_frame(window_layer));
	bitmap_layer_set_bitmap(tread_min_1_layer, tread_min_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_min_1_layer));


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


// void handle_deinit(AppContextRef ctx) {
//   (void)ctx;
// 
//   bmp_deinit_container(&tread_min_1);
// }

// void pbl_main(void *params) {
//   PebbleAppHandlers handlers = {
//     .init_handler = &handle_init,
//     .deinit_handler = &handle_deinit,
//     .tick_info = {
//       .tick_handler = &handle_second_tick,
//       .tick_units = SECOND_UNIT
//     }
//   };
//   app_event_loop(params, &handlers);
// }
