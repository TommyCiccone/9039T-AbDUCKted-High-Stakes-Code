/* Upload Command:
pros upload --icon planet --slot 1 --name "abDUCKted" --description "Patch 2024-03-03-0001"
// Command for PROS termianl to upload program to V5 Brain with correct Name, Description, and Icon
*/

// Include Libraries
#include "main.h"													// Include PROS Core Library
#include "project/auton.hpp"										// Include Auton Header File
#include <iostream>
#include <thread>
#include "liblvgl/lvgl.h"											// Include LVGL, a lightweight graphics library
#include "lemlib/api.hpp"											// Include LemLib, for easy autonomous and odometry)

// Device Declarations
pros::Controller master(pros::E_CONTROLLER_MASTER); 				// Creates Primary Controller
pros::MotorGroup left_mg({-1, -2, 3}, pros::MotorGearset::blue);	// Creates Left Drive Motor Group with ports 1, 2, 3
pros::MotorGroup right_mg({4, 5, -6}, pros::MotorGearset::blue);  	// Creates Right Drive Motor Group with ports 4, 5, 6
pros::Motor conveyor(-7);                                           // Creates Conveyor Motor with port 7
pros::Motor intake(8);pros::Motor lady_brown(-10);					// Creates Lady Brown Motor with port 10
pros::ADIDigitalOut clamp ('A');									// Initialize Goal Clamp Piston on port A
pros::ADIDigitalOut doinker ('B');									// Initialize Doinker Piston on port B
pros::Imu inertial(12);												// Initialize Inertial Sensor on port 12					
pros::Rotation hTrack(11);											// Initialize Rotation Sensor for Horizontal Tracking Wheel on Port 11.
pros::Rotation lbSensor(13);										// Initialize Rotation Sensor for Lady Brown Pivot axle on port 13.
pros::Optical colorSensor(14);										// Initialize Color Sensor on port 14				

// LemLib Declarations [From LemLib Template]
// Declare Drivetrain
lemlib::Drivetrain drivetrain(&left_mg, 							// left motor group
                              &right_mg, 							// right motor group
                              12.625, 								// 12.625 inch track width
                              lemlib::Omniwheel::NEW_325, 			// using new 3.25" omnis
                              450,						 			// drivetrain rpm is 450
                              2 									// horizontal drift is 2 
);

lemlib::TrackingWheel horizontal1(&hTrack, lemlib::Omniwheel::NEW_325, -1.5); // Define horizontal tracking wheel for use in odometry

// Declare Sensors
lemlib::OdomSensors sensors(nullptr, 								// vertical tracking wheel 1, set to null
                            nullptr,								// vertical tracking wheel 2, set to null
                            &horizontal1, 							// horizontal tracking wheel 1, set to horizontal rotation sensor. 
                            nullptr, 								// horizontal tracking wheel 2, set to null
                            &inertial 								// inertial sensor, set to null
);
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
int colorIndex = 0;													// Declares an int for storing the selected color.
int autonOpticsColorRangeLowerBound = 0;										// Declares an int for storing the selected color range.
int autonOpticsColorRangeUpperBound = 0;										// Declares an int for storing the selected color range.
int lbColorRangeLowerBound = 0;												// Declares an int for storing the selected color range.
int lbColorRangeUpperBound = 0;												// Declares an int for storing the selected color range.
int currentColor = 500;
lv_obj_t * activeScreen = lv_obj_create(lv_scr_act());				// Creates activeScreen parent object
lv_obj_t * autonRoller = lv_roller_create(activeScreen);			// Creates a roller object as a child of the activeScreen parent
lv_obj_t * colorRoller = lv_roller_create(activeScreen);			// Creates a roller object as a child of the activeScreen parent`

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

// When Start
void initialize() {
	inertial.reset();
	hTrack.reset();
	lbSensor.reset();
	chassis.calibrate();
	lady_brown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	lady_brown.set_zero_position(0);
	chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);				// Set Brake Mode to Brake
	master.rumble("---------------");								// Rumble Controller to Indicate Calibration Complete

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
}

namespace ladyBrown {                                       // Declare namespace for Lady Brown           
    int moveToTarget(int targetPosition) {             	// Declare function moveToTarget ladyBrown::moveToTarget
        int currentPosition = lbSensor.get_position();      // Get current position of rotation sensor
        int error = currentPosition - targetPosition;       // Calculate error between current position and target position
        int threshold = 5;                                  // Set Threshold for error

        while (abs(error) > threshold) {                       // If error is greater than threshold
            currentPosition = lbSensor.get_position();      // Get current position of rotation sensor
            error = currentPosition - targetPosition;       // Calculate error between current position and target position
            
			if (error > 0) {
				lady_brown.move_relative(-10, 127);
				pros::delay(1000);
			};	// Move Lady Brown Motor at motorSpeed
			if (error < 0) {
				lady_brown.move_relative(10, 127);
				pros::delay(1000);
			};	// Move Lady Brown Motor at motorSpeed
		
        };

        lady_brown.move_velocity(0);                        // Stop Lady Brown Motor

		return 0;
	};
};

// When Disabled
void disabled() {}

// When Connect to Field Control
void competition_initialize() {}

bool didAuton = false;

// When Autonomous
void autonomous() {
	didAuton = true;
//	lady_brown.move_relative(720, 127);

	autonIndex = lv_roller_get_selected(autonRoller);				// Sets autonIndex to index of currently selected roller item
	colorIndex = lv_roller_get_selected(colorRoller);				// Sets colorIndex to index of currently selected roller item

	if (colorIndex == 0) {
		autonOpticsColorRangeLowerBound = 1000; //not real colors; will not stop
		autonOpticsColorRangeUpperBound = 1001;
		lbColorRangeLowerBound = 1000; //not real colors; will not activate lady brown.
		lbColorRangeUpperBound = 1001;
	}
	if (colorIndex == 1) {
		autonOpticsColorRangeLowerBound = 220; //will stop if sees blue
		autonOpticsColorRangeUpperBound = 235;
		lbColorRangeLowerBound = 350; //will activate lb when red
		lbColorRangeUpperBound = 5;
	}
	if (colorIndex == 2) {
		autonOpticsColorRangeLowerBound = 350; //will stop if sees red
		autonOpticsColorRangeUpperBound = 5;
		lbColorRangeLowerBound = 220; //will activate lb when blue
		lbColorRangeUpperBound = 235;
	}


	if (autonIndex == 0) {};										// Runs auton routine if autonIndex = a number. (0 --> disabled)
	if (autonIndex == 1) {
		chassis.setPose(52, 15.5, 0);
		pros::delay(1000);
		chassis.moveToPoint(52, 0, 3000, {.forwards = false});
		pros::delay(1000);
		chassis.turnToHeading(270, 2000);
		pros::delay(1000);
		chassis.moveToPoint(60, 0, 3000, {.forwards = false, .maxSpeed = 25});
		pros::delay(1000);
		lady_brown.move_relative(4320, 127);

	};											// Runs auton routine if autonIndex = a number. (1 --> ganza1);										
	if (autonIndex == 2) {};										// Runs auton routine if autonIndex = a number. (2 --> ganza2)
	if (autonIndex == 3) {											// Runs auton routine if autonIndex = a number. (1 --> sugar1)
		chassis.setPose(60, -24, 270);									// Set Starting Position
		clamp.set_value(true);											// Extended Clamp
		chassis.moveToPoint(24, -24, 5000, {.maxSpeed = 84});			// Drive to Goal 
		pros::delay(2000);												// Wait
		clamp.set_value(false);											// Clamp Goal (retract clamp)
		pros::delay(500);												// Wait
		intake.move(127);
		conveyor.move(127);											// Deposit Preload Ring onto Goal
		pros::delay(750);												// Wait
		chassis.turnToHeading(0, 2000);									// Turn to Face Ring Stack South of Goal
		chassis.moveToPoint(24, -46, 5000, {.forwards = false});		// Drive to Ring Stack, Knock Off Top Ring
		pros::delay(1250);												// Wait
		chassis.moveToPoint(24, -54, 5000, {.forwards = false});		// Drive Reverse to Assist Intake
		pros::delay(3000);												// Wait
		intake.move(0);
    	conveyor.move(0);												// Stop Intake
		chassis.turnToHeading(180, 2000);								// Turn to Face Ladder
		chassis.moveToPoint(24, -4, 5000, {.forwards = false});			// Drive to Ladder
		intake.move(127);
		conveyor.move(127);											// Start intake again just in case ring doesn't make it to goal
	};										
	if (autonIndex == 4) {											// Runs auton routine if autonIndex = a number. (2 --> sugar2)
		chassis.setPose(60, 24, 270);									// Turn to face goal
		clamp.set_value(true);											// Extend Clamp
		chassis.moveToPoint(24, 24, 5000, {.maxSpeed = 84});			// Slowly approach goal, to avoid pushing it away
		pros::delay(2000);												// Wait
		clamp.set_value(false);											// Clamp Goal
		pros::delay(500);												// Wait
		intake.move(127);
		conveyor.move(127);											// Deposit Preload ring onto goal
		pros::delay(750);												// Wait
		chassis.turnToHeading(180, 2000);								// Turn to face ring stack North of goal
		chassis.moveToPoint(24, 46, 5000, {.forwards = false});			// Drive to ring stack
		pros::delay(1250);												// Wait
		chassis.moveToPoint(24, 54, 5000, {.forwards = false});			// Drive reverse to assist intake
		pros::delay(3000);												// Wait
		intake.move(0);
        conveyor.move(0);												// Stop
		chassis.turnToHeading(0, 2000);									// Turn to face ladder
		chassis.moveToPoint(24, 4, 5000, {.forwards = false});			// Drive to ladder
		intake.move(127);
		conveyor.move(127);											// Start intake again just in case ring doesn't make it to goal
	};
	if (autonIndex == 5) {
		lady_brown.move(127);
	}
	if (autonIndex == 7) {											// Runs auton routine if autonIndex = a number. (5 --> old1)
		clamp.set_value(true);											// Extended Clamp
		chassis.setPose(60, -24, 270);									// Set Starting Position
		chassis.moveToPoint(38.25, -15, 5000);							// Drive Part Way to Goal
		chassis.turnToHeading(237, 2000);								// Turn to Face Goal
		chassis.moveToPoint(23.16, -23.45, 5000);								// Drive to Goal 
		pros::delay(1000);												// Wait
		clamp.set_value(false);											// Clamp Goal (retract clamp)
		pros::delay(500);
		intake.move_relative(1440, 127);								// Deposit Preload Ring onto Goal
		conveyor.move_relative(1440, 127);								// Deposit Preload ring onto goal
		pros::delay(2000);												// Wait
		chassis.turnToHeading(180, 2000);
		chassis.turnToHeading(0, 2000);									// Turn to Face Ring Stack South of Goal
		chassis.moveToPoint(24, -46, 5000, {.forwards = false});		// Drive to Ring Stack, Knock Off Top Ring
		pros::delay(2000);												// Wait
		intake.move(127);
		conveyor.move(127);											// Intake Bottom Ring of Stack, Load onto MoGo
		chassis.moveToPoint(24, -54, 5000, {.forwards = false});		// Drive Reverse to Assist Intake
		pros::delay(10000);												// Wait
		intake.move(0);
    	conveyor.move(0);												// Stop Intake
	};											
	if (autonIndex == 8) {											// Runs auton routine if autonIndex = a number. (6 --> old2)
		clamp.set_value(true);											// Extend Clamp
		chassis.setPose(60, 24, 270);									// Set Start Position
		chassis.turnToHeading(242.5, 2000);								// Turn
		chassis.moveToPoint(38.25, 15, 5000);							// Drive part way to goal
		chassis.turnToHeading(303, 2000);								// Turn to face goal
		chassis.moveToPoint(23.16, 23.45, 8000);						// Slowly approach goal, to avoid pushing it away
		pros::delay(1000);												// Wait
		clamp.set_value(false);											// Clamp Goal
		pros::delay(500);
		intake.move_relative(1440, 127);								// Deposit Preload ring onto goal
		conveyor.move_relative(1440, 127);								// Deposit Preload ring onto goal
		pros::delay(2000);												// Wait
		chassis.turnToHeading(0, 2000);									
		chassis.turnToHeading(180, 2000);								// Turn to face ring stack North of goal
		chassis.moveToPoint(24, 46, 5000, {.forwards = false});			// Drive to ring stack
		pros::delay(2000);												// Wait
		intake.move(127);
		conveyor.move(127);											// Intake and deposit rings
		chassis.moveToPoint(24, 54, 5000, {.forwards = false});			// Drive reverse to assist intake
		pros::delay(10000);												// Wait
		intake.move(0);
        conveyor.move(0);												// Stop
	};											
	if (autonIndex == 9) {												// Runs auton routine if autonIndex = a number. (7 --> clearLine)
		chassis.setPose(0, 0, 0);
		chassis.moveToPoint(0, 36, 5000);								// Drive away from line
	};											
	if (autonIndex == 10) {												// Runs auton routine if autonIndex = a number. (8 --> skillsAuton)
		chassis.setPose(-61, -24, 90);										// Set Starting Position		
		clamp.set_value(true);												// Open Clamp
		pros::delay(300);													// Wait				
		chassis.moveToPoint(-48, -24, 5000, {.maxSpeed = 50});				// Drive to closest goal, careful not to push it away
		pros::delay(1250);													// Wait
		clamp.set_value(false);												// Clamp Goal	
		intake.move(127);
		conveyor.move(127);												// Load first ring onto goal
		pros::delay(125);													// Wait
		chassis.turnToHeading(-90, 2000);									// Turn away from next ring (we drive backwards to it)
		chassis.moveToPoint(-24, -24, 2000, {.forwards = false});			// Drive backwards to next ring
		pros::delay(2000);													// Wait
		chassis.turnToHeading(0, 2000);										// Turn to next ring, etc.	
		chassis.moveToPoint(-24, -48, 3000, {.forwards = false});			// Drive to next ring, etc.
		pros::delay(2000);
		chassis.turnToHeading(90, 2000);
		chassis.moveToPoint(-48, -48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.moveToPoint(-60, -48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(315, 2000);
		chassis.moveToPoint(-48, -58, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(270, 2000);									// Face nearest corner
		chassis.moveToPoint(-64, -64, 3000);								// Drive to corner				
		pros::delay(1000);													// Wait			
		chassis.turnToHeading(225, 2000);									// Turn partially
		clamp.set_value(true);												// Open Clamp				
		chassis.moveToPoint(-48, -48, 3000, {.forwards = false});			// Drive out of corner
		chassis.moveToPoint(-56, 5, 4000);									// Drive towards next mobile goal		
		chassis.turnToHeading(26, 3000);									// Turn to face next mobile goal
		chassis.moveToPoint(-48, 24, 2000, {.maxSpeed = 50});				// Slowly approach next mobile goal
		clamp.set_value(false);												// Clamp next mobile goal
		pros::delay(125);													// repeat above steps for this goal
		chassis.turnToHeading(-90, 2000);
		chassis.moveToPoint(-24, 24, 2000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(0, 2000);
		chassis.moveToPoint(-24, 48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(90, 2000);
		chassis.moveToPoint(-48, 48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.moveToPoint(-60, 48, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(315, 2000);
		chassis.moveToPoint(-48, 58, 3000, {.forwards = false});
		pros::delay(2000);
		chassis.turnToHeading(270, 2000);
		chassis.moveToPoint(-64, 64, 3000);
		pros::delay(1000);
		chassis.turnToHeading(225, 2000);
		clamp.set_value(true);
		chassis.moveToPoint(-48, -48, 3000, {.forwards = false});
		intake.move(0);
          conveyor.move(0);													// Stop Intake
		chassis.moveToPoint(41, 27, 3500);									// Drive to one of the mogos with blue ring on it
		chassis.moveToPoint(50, 24, 3000);									// Approach the goal slowly
		clamp.set_value(false);												// Clamp the goal						
		pros::delay(125);													// Wait
		chassis.moveToPoint(64, 64, 3000);									// Drive to the corner			
		pros::delay(500);													// Wait				
		clamp.set_value(true);												// Open Clamp				
		pros::delay(300);													// Wait				
		chassis.moveToPoint(48, 48, 3000, {.forwards = false});				// Drive out of corner
		chassis.moveToPoint(48, 12, 2500);									// Drive to the middle mogo (instead of other one with blue ring to avoid crash)					
		chassis.moveToPoint(48, 0, 2500, {.maxSpeed = 50});					// Approach the goal slowly
		clamp.set_value(false);												// Clamp the goal
		chassis.moveToPoint(64, -64, 3000);									// Drive to the corner
		pros::delay(500);													// Wait				
		clamp.set_value(true);												// Open Clamp				
		pros::delay(300);													// Wait
		chassis.moveToPoint(48, -48, 3000, {.forwards = false});			// Drive out of corner
	};
}

/*void ladyBrownAutomation(void* param) {
    intake.move(127);
conveyor.move(127); // Spin intake motors forward
    lady_brown.move_absolute(20, 127); // Keep Lady Brown motor up

    while (true) {
		ladyBrownTaskRunning = true; // Set the task running flag to true
        int detectedColor = colorSensor.get_hue(); // Get the detected color hue

        // Check if the detected color is within the specified range
        if ((detectedColor >= lbColorRangeLowerBound && detectedColor <= lbColorRangeUpperBound) ||
            (lbColorRangeLowerBound > lbColorRangeUpperBound && 
             (detectedColor >= lbColorRangeLowerBound || detectedColor <= lbColorRangeUpperBound))) {
            pros::delay(1000); // Wait for a set amount of time
            break; // Stop the loop
        };

        // Check if the Y button is pressed to cancel the operation
        if (master.get_digital(DIGITAL_Y)) {
            break; // Stop the loop
        };

        pros::delay(10); // Small delay to prevent CPU overload
    };

    intake.move(0);
          conveyor.move(0); // Stop intake motors
    lady_brown.move_absolute(0, 127); // Move Lady Brown motor back to zero position
	ladyBrownTaskRunning = false; // Set the task running flag to false
}
*/
bool ladyBrownTaskRunning = false;

// When Driver Control
void opcontrol() {
	if (didAuton = true) {
		lady_brown.move_relative(-720, 127);
	};

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
			intake.move(127);
conveyor.move(127);									// Spin Motors Forward
		};
		if (master.get_digital(DIGITAL_R2)) {						// Is controller L2 Pressed?
			intake.move(-127);
        	conveyor.move(-127);									// Spin Motors Reverse
		}; 
		if (!master.get_digital(DIGITAL_R1) && !master.get_digital(DIGITAL_R2) && !ladyBrownTaskRunning) {	// Otherwise
			intake.move(0);
          conveyor.move(0);										// Stop Motors
		};

		// Doinker Control
		if (master.get_digital(DIGITAL_UP)) {						// Is Up Arrow Pressed?
			doinker.set_value(true);									// Set Solenoid to True	
		};
		if (master.get_digital(DIGITAL_DOWN)) {						// Is Down Arrow Pressed?
			doinker.set_value(false);									// Set Solenoid to False
		};
	if (master.get_digital(DIGITAL_B) && !ladyBrownTaskRunning) {
            ladyBrownTaskRunning = true;
            intake.move(127);
        	conveyor.move(48); // Spin intake motors forward
            lady_brown.move_relative(720, 127); // Keep Lady Brown motor up
        };

	if (ladyBrownTaskRunning) {
	int detectedColor = colorSensor.get_hue(); // Get the detected color hue

	// Check if the detected color is within the specified range
	if ((detectedColor >= lbColorRangeLowerBound && detectedColor <= lbColorRangeUpperBound) ||
		(lbColorRangeLowerBound > lbColorRangeUpperBound && 
		 (detectedColor >= lbColorRangeLowerBound || detectedColor <= lbColorRangeUpperBound))) {
		pros::delay(410); // Wait for a set amount of time
		ladyBrownTaskRunning = false; // Stop the task
	}

	// Check if the Y button is pressed to cancel the operation
	if (master.get_digital(DIGITAL_Y)) {
		ladyBrownTaskRunning = false; // Stop the task
	}

	if (!ladyBrownTaskRunning) {
		intake.move(0);
          conveyor.move(0); // Stop intake motors
		lady_brown.move_relative(-720, 127); // Move Lady Brown motor back to zero position
	}
}

/*
		// Lady Brown Motor Control - Hold Mode
		if (master.get_digital(DIGITAL_A)) {						// Is Controller L1 Pressed?
			lady_brown.move_velocity(20);									// Spin Motors Forward
		};
		if (master.get_digital(DIGITAL_B)) {						// Is controller L2 Pressed?
			lady_brown.move_velocity(-20);									// Spin Motors Reverse
		}; 
		if (!master.get_digital(DIGITAL_A) && !master.get_digital(DIGITAL_B)) {	// Otherwise
			lady_brown.move_velocity(0);										// Stop Motors
		};
*/
		pros::delay(10);                               				// Wait 10ms

	};
}