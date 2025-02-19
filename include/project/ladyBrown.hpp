#pragma once                                                // Only include once

#include "main.h"                                           // Include PROS Core Library                   

extern pros::Motor lady_brown;                              // Declare Lady Brown Motor (external, declared in main.cpp)
extern pros::Rotation lbSensor;                             // Declare rotation sensor (external)

namespace ladyBrown {                                       // Declare namespace for Lady Brown           
    void moveToTarget(int targetPosition = 0) {             // Declare function moveToTarget ladyBrown::moveToTarget
        int currentPosition = lbSensor.get_position();      // Get current position of rotation sensor
        int error = currentPosition - targetPosition;       // Calculate error between current position and target position
        int threshold = 5;                                  // Set Threshold for error
        int motorSpeed = 0;                                 // Declare motorSpeed variable

        int kp = .5;                                        // Set Proportional Gain        
//      int ki = 0;
//      int kd = 0;

        if (abs(error) > threshold) {                       // If error is greater than threshold
            currentPosition = lbSensor.get_position();      // Get current position of rotation sensor
            error = currentPosition - targetPosition;       // Calculate error between current position and target position
            
            motorSpeed = error * kp;                        // Calculate motorSpeed based on error and kp

            lady_brown.move_velocity(motorSpeed);           // Move Lady Brown Motor at motorSpeed
        };

        lady_brown.move_velocity(0);                        // Stop Lady Brown Motor
    };
}