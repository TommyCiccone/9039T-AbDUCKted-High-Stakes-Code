// pros upload --icon 'planet'

// Include Libraries
#include "main.h"

// Include other Files
	//note to self - figure out how to reference other files for ui and other stuff

// When Center Button Pressed
void on_center_button() {}

// When Start
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "9039T - AbDUCKted");
	pros::lcd::register_btn1_cb(on_center_button);
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
	pros::MotorGroup left_mg({1, 2});						// Creates Left Motor Group with ports 1 & 2
	pros::MotorGroup right_mg({3, 4});  					// Creates Right Motor Group with ports 3 & 4


	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
		int left = master.get_analog(ANALOG_LEFT_Y);   	 	// Gets Left Stick Up/Down Value
		int right = master.get_analog(ANALOG_RIGHT_Y);  	// Gets Right Stick Up/Down Value
		left_mg.move(left);		                  			// Sets Left Motor Group Speed
		right_mg.move(right);                     			// Sets Right Motor Group Speed
		pros::delay(10);                               		// Wait 10ms
	}
}