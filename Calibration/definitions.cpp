//
// Created by marc on 25.01.18.
//


#include "definitions.hpp"

std::string returnRoot(){

    std::string root;

    #ifdef __APPLE__

        root = "/Users/marchaubenstock/Workspace/Xcode/Calibration/";

    #elif __linux__

        root = "/home/marc/Workspace/Vision/Realsense_Calibration/";

    #endif

    return root;
}