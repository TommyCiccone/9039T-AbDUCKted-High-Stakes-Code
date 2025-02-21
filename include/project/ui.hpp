#pragma once

#include "main.h"                                                   // Include PROS Core Library
#include "liblvgl/lvgl.h"											// Include LVGL, a lightweight graphics library

#include "project/uiElements.hpp"										// Include Auton Header File
#include "project/uiEventHandler.hpp"                               // Include UI Event Handler Header File

extern int autonIndex = 0;											// Declares an int for storing the selected auton routine.
extern int colorIndex = 0;											// Declares an int for storing the selected color.
extern lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());		// Creates activeScreen parent object
extern lv_obj_t * autonRoller = lv_roller_create(activeScreen);		// Creates a roller object as a child of the activeScreen parent
extern lv_obj_t * colorRoller = lv_roller_create(activeScreen);		// Creates a roller object as a child of the activeScreen parent

extern void color_roller_event_handler(lv_event_t * e);

void setupUI() {
    lv_obj_set_style_text_font(										// Set font size to 36 pt.
        lv_scr_act(), 
        &lv_font_montserrat_18, 
        LV_PART_MAIN | LV_STATE_DEFAULT
    );
    // Configure Auton Roller
    lv_obj_set_size(activeScreen, 480, 220);						// Configure size & position of activeScreen Parent
    lv_obj_center(activeScreen);
    lv_roller_set_options(											// Configure Roller
        autonRoller, 
        auton::autonNames.c_str(), 
        LV_ROLLER_MODE_NORMAL
    );
    lv_roller_set_visible_row_count(autonRoller, 4);
    lv_obj_set_style_bg_color(										// Set highlight color of selected choice to a bold yellow
        autonRoller, 
        lv_color_hex(0xFFFF00), 
        LV_PART_SELECTED
    );
    lv_obj_set_style_text_color(									// Set text color to black
        autonRoller, 
        lv_color_hex(0x000000), 
        LV_PART_SELECTED
    );
    lv_obj_set_size(autonRoller, 238, 220);							// Configure size & position of roller object
    lv_obj_align(autonRoller, LV_ALIGN_LEFT_MID, 0, 0);

    // Configure Color Roller
    lv_roller_set_options(
        colorRoller,
        color::colorNames.c_str(),
        LV_ROLLER_MODE_NORMAL
    );
    lv_roller_set_visible_row_count(colorRoller, 2);
    lv_obj_set_style_bg_color(
        colorRoller,
        lv_color_hex(0xFFFF00),
        LV_PART_SELECTED
    );
    lv_obj_set_style_text_color(
        colorRoller,
        lv_color_hex(0x000000),
        LV_PART_SELECTED
    );
    lv_obj_set_size(colorRoller, 238, 220);						// Configure size & position of roller object
    lv_obj_align(colorRoller, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(colorRoller, color_roller_event_handler, LV_EVENT_ALL, NULL);
};
