#pragma once                                               // Library is only called once

#include <string>                                          // Include string library

namespace auton {                                          // Creates "auton" namespace
    const std::string autonNames =                         // List of autons to be selected for display
        "Disabled\nRegionals Left\nRegionals Right\nSugar Left\nSugar Right\n*Tommy's Shenanigans\n*Not Tommy's Shenanigans\nLeft Old\nRight Old\nClear Line\nSkills Auton\nSkills Beta\nSkills 20\nSkills 5";                           
}

namespace color {
    const std::string colorNames =                         // List of colors to be selected for display
        "Disable Color Sensing\nRed Alliance\nBlue Alliance";
};