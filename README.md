**Vex V5 Robot Code for Team 9039T - AbDUCKted for the 2024-2025 season, V5RC High Stakes**

File Descriptions:

    firmware                  Folder containing Vex V5 brain firmware and related files, created automatically

        libc.a                    Linux library containing c/c++ libraries
    
        liblvgl.a                 C Graphics Library for creating guis on the brain
    
        libm.a                    Advanced mathematical functions like trig and log
    
        libpros.a                 Pros library
    
        v5-common.ld              V5 Firmware
    
        v5-hot.ld                 V5 Firmware
    
        v5.ld                     V5 Firmware
    
    include                   Folder containing included libraries

        liblvgl                   Folder containing many graphics library, created automatically
    
        pros                      Folder containing many PROS libraries, created automatically

        project                   Folder containing custom headers referenced by the .cpp files

            auton.hpp                Custom header containing a list of auton routines
    
        api.h                     Core PROS Library, created automatically
    
        main.h                    Core PROS Library, created automatically
    
    src                       Folder containing all C++ files

        main.cpp                  Main C++ file, only file that runs on startup, references all other files
    
    .DS_store                 Created and used by macOS, not relevant to code

    .gitignore                List of files ignored by Git

    Makefile                  Advanced configuration file created automatically by PROS

    common.mk                 Advanced configuration file created automatically by PROS

    project.pros              Manifest file created by PROS
