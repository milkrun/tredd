// copyright (c) 2014 Francis Stanbach Jr. all rights reserved.

#include "pebble.h"

static Window *window;
static MenuLayer *menu_layer;

static GBitmap *background_image;
static BitmapLayer *background_layer;

// you need 3 minute treads to appear as a continuous tread
// with animation

static GBitmap *tread_min_1_image;
static BitmapLayer *tread_min_1_layer;
static BitmapLayer *tread_min_2_layer;
static BitmapLayer *tread_min_3_layer;

static PropertyAnimation *minute_animation_1;
static PropertyAnimation *minute_animation_2;
static PropertyAnimation *minute_animation_3;

// 2 hour treads

static GBitmap *tread_hour_1_image;
static BitmapLayer *tread_hour_1_layer;
static BitmapLayer *tread_hour_2_layer;
static BitmapLayer *tread_hour_3_layer;

static PropertyAnimation *hour_animation_1;
static PropertyAnimation *hour_animation_2;
static PropertyAnimation *hour_animation_3;

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


#define BUZZ_KEY 100


#define BUZZ_NOT 	0
#define BUZZ_9to9  	1

static int buzz_mode = BUZZ_9to9;

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

static int last_hour = 0;
static int last_minute = 0;


static void show_this(int hour, int minutes);



// I want to vibe the watch 5 seconds before the hour
// between 9am and 9pm

static void hour_vibe(struct tm *current_time) {

	if (buzz_mode == BUZZ_NOT)
		return;

	if (current_time->tm_hour < 8) 
		return;

	if (current_time->tm_hour > 20) 
		return;

	if (current_time->tm_min != 59)
		return;

	if (current_time->tm_sec != 55)
		return;
		
	vibes_short_pulse();

}

static struct tm *last_time;

static int hour_test = 0;

static void update_display(struct tm *current_time) {

	last_time = current_time;

	if (mode == TIME) {
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: tick --- %d %d %d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
  	} else if (mode == DATE) {
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: date ---");
  	} else if (mode == CHRONO) {
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: chrono ---");
  	} 

	if (mode == DATE) {
		date_timer--;
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: date_timer %d", date_timer);
		if (date_timer == 0) {
  			APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: switch to time mode");
  			mode = TIME;
		}
		
		int month = current_time->tm_mon + 1;
		if (month == 12) {
			month = 0;
		}
		
		show_this(month, current_time->tm_mday);

	} else if (mode == TIME) { 
	
		// show time
	
// 		hour_test++;
// 		hour_test %=12;
// 		show_this(hour_test, current_time->tm_sec);
		
// 		show_this(current_time->tm_hour, current_time->tm_sec);

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
  
static int minute_y(int minute) {
	return 84 - (6 * minute);  
}
  
static void show_this(int hour, int minute) {
 
  if (minute != last_minute) {

		// special case from 59 --> 0

		int fake_minute = minute;

  		if (minute == 0 && last_minute == 59) {
  			fake_minute = 60;
  		}
  
  
	  	// use 3 treads so there is never a gap above or below

		int yy = minute_y(last_minute);
		int yy2 = yy - min_height;
		int yy3 = yy + min_height;
		
		GRect min_start_rect_1 = (GRect) {
			.origin = { .x = min_x, .y = yy },
			.size = tread_min_1_image->bounds.size
		};

		GRect min_start_rect_2 = (GRect) {
			.origin = { .x = min_x, .y = yy2 },
			.size = tread_min_1_image->bounds.size
		};

		GRect min_start_rect_3 = (GRect) {
			.origin = { .x = min_x, .y = yy3 },
			.size = tread_min_1_image->bounds.size
		};

		int y = minute_y(fake_minute);
		int y2 = y - min_height;
		int y3 = y + min_height;
		
		GRect min_dest_rect_1 = (GRect) {
			.origin = { .x = min_x, .y = y },
			.size = tread_min_1_image->bounds.size
		};

		GRect min_dest_rect_2 = (GRect) {
			.origin = { .x = min_x, .y = y2 },
			.size = tread_min_1_image->bounds.size
		};

		GRect min_dest_rect_3 = (GRect) {
			.origin = { .x = min_x, .y = y3 },
			.size = tread_min_1_image->bounds.size
		};

		if (minute_animation_1 != NULL) {
			animation_destroy((Animation*) minute_animation_1);
		}
		if (minute_animation_2 != NULL) {
			animation_destroy((Animation*) minute_animation_2);
		}
		if (minute_animation_3 != NULL) {
			animation_destroy((Animation*) minute_animation_3);
		}
  
  		int anim_time = 500;
  
		minute_animation_1 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_min_1_layer), &min_start_rect_1, &min_dest_rect_1);
		animation_set_duration((Animation*) minute_animation_1, anim_time);
		animation_schedule((Animation*) minute_animation_1);
	
		minute_animation_2 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_min_2_layer), &min_start_rect_2, &min_dest_rect_2);
		animation_set_duration((Animation*) minute_animation_2, anim_time);
		animation_schedule((Animation*) minute_animation_2);
	
		minute_animation_3 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_min_3_layer), &min_start_rect_3, &min_dest_rect_3);
		animation_set_duration((Animation*) minute_animation_3, anim_time);
		animation_schedule((Animation*) minute_animation_3);
  	
  		last_minute = minute;
  	}
  	
  	// finished with minutes
  	
  	if (hour == last_hour) {
  		return;
  	}
  	
  	int fake_hour = hour;
      		
	if (hour == 0 && last_hour == 11) {
		fake_hour = 12;
	}

	int x1 = getHourX(last_hour);
	int x2 = x1 - hour_width;
	int x3 = x1 + hour_width;
    
  	GRect hour_start_rect_1 = (GRect) {
		.origin = { .x = x1, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	GRect hour_start_rect_2 = (GRect) {
		.origin = { .x = x2, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	GRect hour_start_rect_3 = (GRect) {
		.origin = { .x = x3, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	int xx1 = getHourX(fake_hour);
  	int xx2 = xx1 - hour_width;
  	int xx3 = xx1 + hour_width;
  
  	GRect hour_dest_rect_1 = (GRect) {
		.origin = { .x = xx1, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	GRect hour_dest_rect_2 = (GRect) {
		.origin = { .x = xx2, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};

  	GRect hour_dest_rect_3 = (GRect) {
		.origin = { .x = xx3, .y = hour_y },
		.size = tread_hour_1_image->bounds.size
	};


	if (hour_animation_1 != NULL) {
   		animation_destroy((Animation*) hour_animation_1);
   	}
	if (hour_animation_2 != NULL) {
   		animation_destroy((Animation*) hour_animation_2);
   	}
	if (hour_animation_3 != NULL) {
   		animation_destroy((Animation*) hour_animation_3);
   	}
  
	int anim_time = 500;
  
    hour_animation_1 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_hour_1_layer), &hour_start_rect_1, &hour_dest_rect_1);
  	animation_set_duration((Animation*) hour_animation_1, anim_time);
    animation_schedule((Animation*) hour_animation_1);

    hour_animation_2 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_hour_2_layer), &hour_start_rect_2, &hour_dest_rect_2);
  	animation_set_duration((Animation*) hour_animation_2, anim_time);
    animation_schedule((Animation*) hour_animation_2);

    hour_animation_3 = property_animation_create_layer_frame(bitmap_layer_get_layer(tread_hour_3_layer), &hour_start_rect_3, &hour_dest_rect_3);
  	animation_set_duration((Animation*) hour_animation_3, anim_time);
    animation_schedule((Animation*) hour_animation_3);

    last_hour = hour;
  	

  
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
  		date_timer = 5;
  	} else if (mode == DATE) {
  		mode = TIME;
		// mode = CHRONO;
  	}
  	
  	update_display(last_time);
  	
//   	 else if (mode == CHRONO) {
//   		mode = TIME;
//   	} 

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

/////////////////////////////////////////////////////////////////

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}


static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return 2;

//     case 1:
//       return NUM_SECOND_MENU_ITEMS;

    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx,  
			(GRect){ .origin = GPointZero, .size = layer_get_frame((Layer*) cell_layer).size }, 0, GCornersAll);
      menu_cell_basic_header_draw(ctx, cell_layer, "Settings");
      break;

    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "Exit");
      break;
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle
  			graphics_context_set_fill_color(ctx, GColorWhite);
          	graphics_fill_rect(ctx,  
          		(GRect){ .origin = GPointZero, .size = layer_get_frame((Layer*) cell_layer).size }, 0, GCornersAll);
         	
         	if (buzz_mode == BUZZ_NOT) 
         		menu_cell_basic_draw(ctx, cell_layer, "Buzz Hourly", "Off", NULL);
         	else 
         		menu_cell_basic_draw(ctx, cell_layer, "Buzz Hourly", "9AM to 9PM", NULL);
          break;

        case 1:
  			graphics_context_set_fill_color(ctx, GColorWhite);
          	graphics_fill_rect(ctx,  
          		(GRect){ .origin = GPointZero, .size = layer_get_frame((Layer*) cell_layer).size }, 0, GCornersAll);
          menu_cell_basic_draw(ctx, cell_layer, "Exit", "", NULL);
          break;

//         case 2:
//           // Here we use the graphics context to draw something different
//           // In this case, we show a strip of a watchface's background
//           graphics_draw_bitmap_in_rect(ctx, menu_background,
//               (GRect){ .origin = GPointZero, .size = layer_get_frame((Layer*) cell_layer).size });
//           break;
      }
      break;

    case 1:
      switch (cell_index->row) {
        case 0:
          // There is title draw for something more simple than a basic menu item
          menu_cell_title_draw(ctx, cell_layer, "Final Item");
          break;
      }
  }
}

void config_provider(Window *window) ;

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: menu selected");

  // Use the row to specify which item will receive the select action
	switch (cell_index->row) {
		// This is the buzz item
		case 0:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: buzz on hour toggle");
			
         	if (buzz_mode == BUZZ_NOT) {
         		buzz_mode = BUZZ_9to9;
         	} else {
         		buzz_mode = BUZZ_NOT;
         	}
  			
  			// save buzz value to storage	
  			persist_write_int(BUZZ_KEY, buzz_mode);
			
			layer_mark_dirty(menu_layer_get_layer(menu_layer));

		  break;
		  
		case 1:

			APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: exit menu");

			// destroy the menu layer
  			menu_layer_destroy(menu_layer);
  			// set the old click provider
			window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

		  break;
		  
		  
	}



}

void settings_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: starting settings");

	// Create the menu layer
  	Layer *window_layer = window_get_root_layer(window);
  	GRect bounds = layer_get_frame(window_layer);
	menu_layer = menu_layer_create(bounds);

	// Set all the callbacks for the menu layer
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});

	// Bind the menu layer's click config provider to the window for interactivity
	menu_layer_set_click_config_onto_window(menu_layer, window);

	// Add it to the window for display
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));

}

void config_provider(Window *window) {
 	// single click / repeat-on-hold config:
	
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, chrono_start_stop_click_handler);
// 	window_single_click_subscribe(BUTTON_ID_DOWN, chrono_reset_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, settings_handler);

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
		
	GRect hour_frame_2 = (GRect) {
		.origin = { .x = 42, .y = hour_y},
		.size = tread_hour_1_image->bounds.size
	};
	
	tread_hour_2_layer = bitmap_layer_create(hour_frame_2);
	bitmap_layer_set_bitmap(tread_hour_2_layer, tread_hour_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_hour_2_layer));

	hour_animation_2 = NULL;

	// second hour tread
		
	GRect hour_frame_3 = (GRect) {
		.origin = { .x = 42, .y = hour_y},
		.size = tread_hour_1_image->bounds.size
	};
	
	tread_hour_3_layer = bitmap_layer_create(hour_frame_3);
	bitmap_layer_set_bitmap(tread_hour_3_layer, tread_hour_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_hour_3_layer));

	hour_animation_3 = NULL;

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
 
	tread_min_2_layer = bitmap_layer_create(min_frame);
	bitmap_layer_set_bitmap(tread_min_2_layer, tread_min_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_min_2_layer));

	// load 3rd minute tread

	tread_min_3_layer = bitmap_layer_create(min_frame);
	bitmap_layer_set_bitmap(tread_min_3_layer, tread_min_1_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(tread_min_3_layer));

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

	// update the buzz key from persistent storage

	buzz_mode = persist_exists(BUZZ_KEY) ? persist_read_int(BUZZ_KEY) : BUZZ_NOT;

  	if (buzz_mode == BUZZ_NOT) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: buzz is off");
  	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "TREDD: buzz is on");
  	}


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
	layer_remove_from_parent(bitmap_layer_get_layer(tread_min_3_layer));
	bitmap_layer_destroy(tread_min_3_layer);
	gbitmap_destroy(tread_min_1_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_1_layer));
	bitmap_layer_destroy(tread_hour_1_layer);
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_2_layer));
	bitmap_layer_destroy(tread_hour_2_layer);
	layer_remove_from_parent(bitmap_layer_get_layer(tread_hour_3_layer));
	bitmap_layer_destroy(tread_hour_3_layer);
	gbitmap_destroy(tread_hour_1_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(white_image_layer));
	bitmap_layer_destroy(white_image_layer);
	gbitmap_destroy(white_image);

	layer_remove_from_parent(bitmap_layer_get_layer(black_image_layer));
	bitmap_layer_destroy(black_image_layer);
	gbitmap_destroy(black_image);

	property_animation_destroy(hour_animation_1);
	property_animation_destroy(hour_animation_2);
	property_animation_destroy(hour_animation_3);
  
	property_animation_destroy(minute_animation_1);
	property_animation_destroy(minute_animation_2);
	property_animation_destroy(minute_animation_3);
  
	window_destroy(window);

}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

