#pragma once                                               // Library is only called once

#include <string>                                          // Include string library

namespace auton {                                          // Creates "auton" namespace
    enum class AutonSelect : int {                         // List of autons to be selected for internal operations
        disabled,
        filler_routine
    };

const std::string autonNames =                             // List of autons to be selected for display
    "Disabled\nFiller Routine\n";                           
}