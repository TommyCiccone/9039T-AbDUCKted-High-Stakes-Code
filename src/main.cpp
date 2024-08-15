// Include Libraries
#include "main.h"											// Include PROS Core Library

#include "project/ui.hpp"
#include "project/auton.hpp"

#include "liblvgl/lvgl.h"

// Include other Files
	//note to self - figure out how to reference other files for ui and other stuff

// When Center Button Pressed
void on_center_button() {}

// When Start
void initialize() {
	lv_obj_set_style_text_font(lv_scr_act(), &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());
    lv_obj_t * autonRoller = lv_roller_create(activeScreen);

    lv_roller_set_options(autonRoller, auton::autonNames.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(autonRoller, 4);



	lv_obj_set_size(activeScreen, 480, 272);
	lv_obj_center(activeScreen);

	lv_obj_set_size(autonRoller, 400, 232);
    lv_obj_center(autonRoller);
}

// When Disabled
void disabled() {}

// When Connect to Field Control
void competition_initialize() {}

// When Autonomous
void autonomous() {}

// When Driver Control
void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER); 	// Creates Primary Controller
	pros::MotorGroup left_mg({-1, -2});						// Creates Left Motor Group with ports 1 & 2
	pros::MotorGroup right_mg({3, 4});  					// Creates Right Motor Group with ports 3 & 4
	pros::ADIDigitalOut clamp ('A');						// Initialize Goal Clamp Piston


	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  

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

		pros::delay(10);                               		// Wait 10ms
	};
}