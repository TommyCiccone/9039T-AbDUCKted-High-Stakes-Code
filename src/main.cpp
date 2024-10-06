/* Upload Command:
pros upload --icon planet --slot 1 --name "abDUCKted" --description "Patch 2024-10-06-0001"
*/

// Include Libraries
#include "main.h"											// Include PROS Core Library
#include "project/auton.hpp"								// Include Auton Header File
#include "liblvgl/lvgl.h"									// Include LVGL, a lightweight graphics library

// Device Declarations
pros::Controller master(pros::E_CONTROLLER_MASTER); 		// Creates Primary Controller
pros::MotorGroup left_mg({-1, -2, 3});						// Creates Left Drive Motor Group with ports
pros::MotorGroup right_mg({4, 5, -7});  					// Creates Right Drive Motor Group with ports
pros::MotorGroup intake_mg({8, 9});							// Creates Intake Motor Grouo with ports
pros::ADIDigitalOut clamp ('A');							// Initialize Goal Clamp Piston

// UI Declarations
int autonIndex = 0;											// Declares an int for storing the selected auton routine.
lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());		// Creates activeScreen parent object
lv_obj_t * autonRoller = lv_roller_create(activeScreen);	// Creates a roller object as a child of the activeScreen parent

// When Center Button Pressed
void on_center_button() {}

// When Start
void initialize() {
// All code here will be moved to "void competition_initialize() later. at the time of testing I don't have a comp switch."
	lv_obj_set_style_text_font(								// Set font size to 36 pt.
		lv_scr_act(), 
		&lv_font_montserrat_36, 
		LV_PART_MAIN | LV_STATE_DEFAULT
	);
	lv_obj_set_size(activeScreen, 470, 220);				// Configure size & position of activeScreen Parent
	lv_obj_center(activeScreen);
    lv_roller_set_options(									// Configure Roller
		autonRoller, 
		auton::autonNames.c_str(), 
		LV_ROLLER_MODE_NORMAL
	);
    lv_roller_set_visible_row_count(autonRoller, 4);
    lv_obj_set_style_bg_color(								// Set highlight color of selected choice to a bold yellow
		autonRoller, 
		lv_color_hex(0xFFFF00), 
		LV_PART_SELECTED
	);
    lv_obj_set_style_text_color(							// Set text color to black
		autonRoller, 
		lv_color_hex(0x000000), 
		LV_PART_SELECTED
	);
	lv_obj_set_size(autonRoller, 470, 220);					// Configure size & position of roller object
	lv_obj_center(autonRoller);
}

// When Disabled
void disabled() {}

// When Connect to Field Control
void competition_initialize() {}

// When Autonomous
void autonomous() {
	autonIndex = lv_roller_get_selected(autonRoller);		// Sets autonIndex to index of currently selected roller item
	if (autonIndex = 0) {};									// Runs auton routine if autonIndex = a number. (0 --> disabled)
	if (autonIndex = 1) {};									// Runs auton routine if autonIndex = a number. (1 --> red1)
	if (autonIndex = 2) {};									// Runs auton routine if autonIndex = a number. (2 --> red2)
	if (autonIndex = 3) {};									// Runs auton routine if autonIndex = a number. (3 --> blue1)
	if (autonIndex = 4) {};									// Runs auton routine if autonIndex = a number. (4 --> blue2)
}

// When Driver Control
void opcontrol() {
	while (true) {
		// Tank Drive Control Scheme
		int left = master.get_analog(ANALOG_LEFT_Y);   	 	// Gets Left Stick Up/Down Value
		int right = master.get_analog(ANALOG_RIGHT_Y);  	// Gets Right Stick Up/Down Value
		left_mg.move(left);		                  			// Sets Left Motor Group Speed
		right_mg.move(right);                     			// Sets Right Motor Group Speed

		// Goal Clamp Piston Control
		if (master.get_digital(DIGITAL_R1)) {				// Is Controller R1 Pressed?
			clamp.set_value(true);							// Set Solenoid to True
		};
		if (master.get_digital(DIGITAL_R2)) {				// Is Controller R2 Pressed?
			clamp.set_value(false);							// Set Solenoid to False
		};

		// Intake  Motor Control
		if (master.get_digital(DIGITAL_L1)) {				// Is Controller L1 Pressed?
			intake_mg.move(127);							// Spin Motors Forward
		}
		if (master.get_digital(DIGITAL_L2)) {				// Is controller L2 Pressed?
			intake_mg.move(-127);							// Spin Motors Reverse
		} 
		if (!master.get_digital(DIGITAL_L1) && !master.get_digital(DIGITAL_L2)) {	// Otherwise
			intake_mg.move(0);								// Stop Motors
		};

		pros::delay(10);                               		// Wait 10ms
	};
}