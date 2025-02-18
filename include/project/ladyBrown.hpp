#pragma once

#include "main.h"

extern pros::Motor lady_brown;
extern pros::Rotation lbSensor;

namespace ladyBrown {
    void moveToTarget(int targetPosition = 0) {
        int currentPosition = lbSensor.get_position();
        int error = targetPosition - currentPosition;
        int threshold = 5;

        while (abs(error) > threshold) {
            currentPosition = lbSensor.get_position();
            error = targetPosition - currentPosition;

            int motorSpeed = error * 1.2;
            lady_brown.move_velocity(motorSpeed);
            pros::delay(10);
        };
        lady_brown.move_velocity(0);
    };
}