//
//  calibration.hpp
//  Calibration
//
//  Created by Marc Haubenstock on 07.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#ifndef calibration_hpp
#define calibration_hpp

// include the librealsense C++ header file
#include <librealsense/rs.hpp>

// include OpenCV header file
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

#include <stdio.h>

#include "io.hpp"


bool calibrationFromStream();
bool calibrationFromImageSequence(const vector<Mat>& images);

#endif /* calibration_hpp */
