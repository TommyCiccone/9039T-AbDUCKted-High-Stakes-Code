#pragma once                                               // Library is only called once

#include <string>                                          // Include string library

namespace auton {                                          // Creates "auton" namespace
    enum class AutonSelect : int {                         // List of autons to be selected
        disabled,
        test_one
    };

const std::string autonNames = "Disabled\nTest One\n";     // List of autons to be selected
}