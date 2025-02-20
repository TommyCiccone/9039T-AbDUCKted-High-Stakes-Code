#pragma once                                               // Library is only called once

#include <string>                                          // Include string library

namespace auton {                                          // Creates "auton" namespace
    const std::string autonNames =                         // List of autons to be selected for display
        "Disabled\nSugar Left\nSugar Right\nAlly Left\nAlly Right\nLeft Old\nRight Old\nClear Line\nSkills Auton\n";                           
}

namespace color {
    const std::string colorNames =                         // List of colors to be selected for display
        "Disable Color Sensing\nRed Alliance\nBlue Alliance\nBoth Colors\n";
};