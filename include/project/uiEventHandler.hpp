#pragma once

#include "main.h"                                                   // Include PROS Core Library
#include "liblvgl/lvgl.h"											// Include LVGL, a lightweight graphics library"

#include "project/auton.hpp"										// Include Auton Header File
#include "project/ui.hpp"                                           // Include UI Header File

extern int autonIndex = 0;											// Declares an int for storing the selected auton routine.
extern int colorIndex = 0;											// Declares an int for storing the selected color.
extern lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());		// Creates activeScreen parent object
extern lv_obj_t * autonRoller = lv_roller_create(activeScreen);		// Creates a roller object as a child of the activeScreen parent
extern lv_obj_t * colorRoller = lv_roller_create(activeScreen);		// Creates a roller object as a child of the activeScreen parent