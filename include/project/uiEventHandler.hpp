#pragma once

#include "main.h"                                                   // Include PROS Core Library
#include "liblvgl/lvgl.h"											// Include LVGL, a lightweight graphics library"

#include "project/ui.hpp"                                           // Include UI Header File

extern lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());		// Creates activeScreen parent object
extern lv_obj_t * autonRoller = lv_roller_create(activeScreen);		// Creates a roller object as a child of the activeScreen parent
extern lv_obj_t * colorRoller = lv_roller_create(activeScreen);		// Creates a roller object as a child of the activeScreen parent

void color_roller_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * roller = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        int selected = lv_roller_get_selected(roller);
        if (selected == 1) { // Red
            lv_obj_set_style_bg_color(roller, lv_color_hex(0xFF0000), LV_PART_SELECTED);
        } else if (selected == 2) { // Blue
            lv_obj_set_style_bg_color(roller, lv_color_hex(0x0000FF), LV_PART_SELECTED);
        } else {
			lv_obj_set_style_bg_color(roller, lv_color_hex(0xFFFF00), LV_PART_SELECTED);
		};
    };
}