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
bool calibrationFromImageSequence(const vector<Mat>& images, const vector<string>& imagePaths,bool useSuppliedParameters, const Mat& cameraMatrix, const Mat& distCoeffs);
void removeRotationFromTranslation(const vector<Mat>& rvecs,const vector<Mat>& tvecs,vector<Mat>& tvecs_no_rotation);
bool calculateCameraParameters(vector<vector<Point3f>>& objectPoints,vector<vector<Point2f>>& imagePoints,const Mat& cameraMatrix, const Mat& distCoeffs,vector<Mat>& rvecs,vector<Mat>& tvecs);

#endif /* calibration_hpp */
