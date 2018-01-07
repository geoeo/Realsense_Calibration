//
//  main.cpp
//  Calibration
//
//  Created by Marc Haubenstock on 05.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

// include the librealsense C++ header file
//#include <librealsense/rs.hpp>
//
//// include OpenCV header file
#include <opencv2/opencv.hpp>
//#include <opencv2/calib3d.hpp>

// include project headers
#include "calibration.hpp"
#include "io.hpp"

//using namespace std;
//using namespace cv;

//float image_width = 640;
//float image_height = 480;
//float squareSize = 29.0; // in mm
////float squareSize = 1.0; // in mm
//// inner points per row, inner points per column
//Size boardSize = Size(7,5);
//Size imageSize = Size(image_width,image_height);
//int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
//int calibrationFlags = CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_FIX_ASPECT_RATIO; // | CV_CALIB_FIX_FOCAL_LENGTH;

int main()
{
    const int FAIL = 1;
    vector<Mat> images;
    vector<string> imagePaths;
    string dirPath = "/Users/marchaubenstock/Workspace/Xcode/Calibration/Images/";
    bool success = true;

//    success = loadFilesInDirInto(imagePaths,dirPath);
//    if(success)
//        loadImages(imagePaths,images);
//    else
//        return FAIL;
//
//    success = calibrationFromImageSequence(images);
    
    success = calibrationFromStream();
    
    if(success)
        return 0;
    else
        return FAIL;
}
