/* Upload Command:
pros upload --icon planet --slot 1 --name "abDUCKted" --description "Patch 2024-23-23-0001"
// Command for PROS termianl to upload program to V5 Brain with correct Name, Description, and Icon
*/

// Include Libraries
#include "main.h"													// Include PROS Core Library
#include "project/auton.hpp"										// Include Auton Header File
#include "liblvgl/lvgl.h"											// Include LVGL, a lightweight graphics library
#include "lemlib/api.hpp"											// Include LemLib, for easy autonomous and odometry

// Device Declarations
pros::Controller master(pros::E_CONTROLLER_MASTER); 				// Creates Primary Controller
pros::MotorGroup left_mg({-1, -2, 3}, pros::MotorGearset::blue);	// Creates Left Drive Motor Group with ports
pros::MotorGroup right_mg({4, 5, -7}, pros::MotorGearset::blue);  	// Creates Right Drive Motor Group with ports
pros::MotorGroup intake_mg({8, 6});									// Creates Intake Motor Group with ports
pros::ADIDigitalOut clamp ('A');									// Initialize Goal Clamp Piston
pros::Imu inertial(10);

// LemLib Declarations [From LemLib Template]
// Declare Drivetrain
lemlib::Drivetrain drivetrain(&left_mg, 							// left motor group
                              &right_mg, 							// right motor group
                              12.625, 								// 12.625 inch track width
                              lemlib::Omniwheel::NEW_325, 			// using new 3.25" omnis
                              450,						 			// drivetrain rpm is 450
                              2 									// horizontal drift is 2 (for now)
);
// Declare Sensors
lemlib::OdomSensors sensors(nullptr, 								// vertical tracking wheel 1, set to null
                            nullptr,								// vertical tracking wheel 2, set to null
                            nullptr, 								// horizontal tracking wheel 1, set to null
                            nullptr, 								// horizontal tracking wheel 2, set to null
                            &inertial 								// inertial sensor, set to null
);
// *PID is not used until we have an IMU and Odometry Set Up*, we just need these here to declare the chassis
// Declare Lateral PID Controller
lemlib::ControllerSettings lateral_controller(10, 					// proportional gain (kP)
                                              0, 					// integral gain (kI)
                                              3,					// derivative gain (kD)
                                              3, 					// anti windup
                                              1, 					// small error range, in inches
                                              100,					// small error range timeout, in milliseconds
                                              3, 					// large error range, in inches
                                              500, 					// large error range timeout, in milliseconds
                                              20 					// maximum acceleration (slew)
);
// Declare Angular PID Controller
lemlib::ControllerSettings angular_controller(2, 					// proportional gain (kP)
                                              1, 					// integral gain (kI)
                                              10, 					// derivative gain (kD)
                                              3, 					// anti windup
                                              1, 					// small error range, in degrees
                                              100, 					// small error range timeout, in milliseconds
                                              3, 					// large error range, in degrees
                                              500, 					// large error range timeout, in milliseconds
                                              84 					// maximum acceleration (slew)
);
// Declare Chassis
lemlib::Chassis chassis(drivetrain, 								// drivetrain settings
                        lateral_controller, 						// lateral PID settings
                        angular_controller,				 			// angular PID settings
                        sensors 									// odometry sensors
);

// UI Declarations
int autonIndex = 0;													// Declares an int for storing the selected auton routine.
lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());				// Creates activeScreen parent object
lv_obj_t * autonRoller = lv_roller_create(activeScreen);			// Creates a roller object as a child of the activeScreen parent

// When Start
void initialize() {
	inertial.reset();
	chassis.calibrate();
	master.rumble("---");

// All code here will be moved to "void competition_initialize() later. at the time of testing I don't have a comp switch."
	lv_obj_set_style_text_font(										// Set font size to 36 pt.
		lv_scr_act(), 
		&lv_font_montserrat_36, 
		LV_PART_MAIN | LV_STATE_DEFAULT
	);
	lv_obj_set_size(activeScreen, 470, 220);						// Configure size & position of activeScreen Parent
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
	lv_obj_set_size(autonRoller, 470, 220);							// Configure size & position of roller object
	lv_obj_center(autonRoller);
}

// When Disabled
void disabled() {}

// When Connect to Field Control
void competition_initialize() {}

// When Autonomous
void autonomous() {
	autonIndex = lv_roller_get_selected(autonRoller);				// Sets autonIndex to index of currently selected roller item
	if (autonIndex == 0) {};										// Runs auton routine if autonIndex = a number. (0 --> disabled)
	if (autonIndex == 1) {};										// Runs auton routine if autonIndex = a number. (1 --> red1)
	if (autonIndex == 2) {};										// Runs auton routine if autonIndex = a number. (2 --> red2)
	if (autonIndex == 3) {											// Runs auton routine if autonIndex = a number. (3 --> blue1)
		clamp.set_value(true);											// Extended Clamp
		chassis.setPose(60, -24, 270);									// Set Starting Position
		chassis.turnToHeading(297.5, 2000);								// Turn
		chassis.moveToPoint(38.25, -15, 5000);							// Drive Part Way to Goal
		chassis.turnToHeading(237, 2000);								// Turn to Face Goal
		chassis.moveToPoint(24, -24, 5000);								// Drive to Goal 
		pros::delay(1000);												// Wait
		clamp.set_value(false);											// Clamp Goal (retract clamp)
		pros::delay(500);
		intake_mg.move_relative(1440, 127);								// Deposit Preload Ring onto Goal
		pros::delay(2000);												// Wait
		chassis.turnToHeading(180, 2000);
		chassis.turnToHeading(0, 2000);									// Turn to Face Ring Stack South of Goal
		chassis.moveToPoint(24, -46, 5000, {.forwards = false});		// Drive to Ring Stack, Knock Off Top Ring
		pros::delay(2000);												// Wait
		intake_mg.move(127);											// Intake Bottom Ring of Stack, Load onto MoGo
		chassis.moveToPoint(24, -54, 5000, {.forwards = false});		// Drive Reverse to Assist Intake
		pros::delay(10000);												// Wait
		intake_mg.move(0);												// Stop Intake
	};											
	if (autonIndex == 4) {											// Runs auton routine if autonIndex = a number. (4 --> blue2)
		clamp.set_value(true);											// Extend Clamp
		chassis.setPose(60, 24, 270);									// Set Start Position
		chassis.turnToHeading(242.5, 2000);								// Turn
		chassis.moveToPoint(38.25, 15, 5000);							// Drive part way to goal
		chassis.turnToHeading(303, 2000);								// Turn to face goal
		chassis.moveToPoint(24, 24, 8000);								// Slowly approach goal, to avoid pushing it away
		pros::delay(1000);												// Wait
		clamp.set_value(false);											// Clamp Goal
		pros::delay(500);
		intake_mg.move_relative(1440, 127);								// Deposit Preload ring onto goal
		pros::delay(2000);												// Wait
		chassis.turnToHeading(0, 2000);									
		chassis.turnToHeading(180, 2000);								// Turn to face ring stack North of goal
		chassis.moveToPoint(24, 46, 5000, {.forwards = false});			// Drive to ring stack
		pros::delay(2000);												// Wait
		intake_mg.move(127);											// Intake and deposit rings
		chassis.moveToPoint(24, 54, 5000, {.forwards = false});			// Drive reverse to assist intake
		pros::delay(10000);												// Wait
		intake_mg.move(0);												// Stop
	};											
	if (autonIndex == 5) {												// Runs auton routine if autonIndex = a number. (5 --> clearLine)
		chassis.setPose(0, 0, 0);
		chassis.moveToPoint(0, 24, 5000);
	};											
	if (autonIndex == 6) {												// Runs auton routine if autonIndex = a number. (5 --> skillsAuton
		chassis.setPose(-58, -30, 58);
		pros::delay(2000);
		clamp.set_value(true);
		pros::delay(2000);
		chassis.moveToPoint(-48, -24, 5000);
		pros::delay(2000);
		clamp.set_value(false);
		intake_mg.move(127);
		pros::delay(2000);
		chassis.turnToHeading(-90, 2000);
		chassis.moveToPoint(-24, -24, 2000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(0, 2000);
		chassis.moveToPoint(-24, -48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(90, 2000);
		chassis.moveToPoint(-48, -48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.moveToPoint(-60, -48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(315, 2000);
		chassis.moveToPoint(-48, -58, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(270, 2000);
		chassis.moveToPoint(-60, -60, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(225, 2000);
		pros::delay(2000);
		clamp.set_value(true);
		chassis.moveToPoint(-48, -48, 3000, {.forwards = false});
		pros::delay(1000000000);
		intake_mg.move(0);
	};
}


// When Driver Control
void opcontrol() {
	while (true) {
		// Tank Drive Control Scheme
		int left = master.get_analog(ANALOG_LEFT_Y);   	 			// Gets Left Stick Up/Down Value
		int right = master.get_analog(ANALOG_RIGHT_Y);  			// Gets Right Stick Up/Down Value
		chassis.tank(left, right);									// Passes Stick Values into LemLib Chassis for Interpretation

		// Goal Clamp Piston Control
		if (master.get_digital(DIGITAL_L1)) {						// Is Controller R1 Pressed?
			clamp.set_value(true);									// Set Solenoid to True
		};
		if (master.get_digital(DIGITAL_L2)) {						// Is Controller R2 Pressed?
			clamp.set_value(false);									// Set Solenoid to False
		};
		
		// Intake Motor Control - Hold Mode
		if (master.get_digital(DIGITAL_R1)) {						// Is Controller L1 Pressed?
			intake_mg.move(127);									// Spin Motors Forward
		};
		if (master.get_digital(DIGITAL_R2)) {						// Is controller L2 Pressed?
			intake_mg.move(-127);									// Spin Motors Reverse
		}; 
		if (!master.get_digital(DIGITAL_R1) && !master.get_digital(DIGITAL_R2)) {	// Otherwise
			intake_mg.move(0);										// Stop Motors
		};

		pros::delay(10);                               				// Wait 10ms
	};
}