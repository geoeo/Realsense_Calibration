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

// include project headers
#include "calibration.hpp"
#include "definitions.hpp"


int main()
{
    const int FAIL = 1;
    vector<Mat> images;
    Mat intrinsics = Mat::eye(3, 3, CV_64F);
    Mat distortion = Mat::zeros(5, 1, CV_64F);;
    
    string intrinsicsFolder = "Calibration_Output/defaults/";
    vector<string> imagePaths;
    bool success;
    string dirPath;
    string root = returnRoot();

    dirPath.append(root).append("Image_Seq_1/");
    // change this to calcuate rms for the default intrinsics on a given image set
    bool useSuppliedParamters = true;

    success = loadFilesInDirInto(imagePaths,dirPath);
    if(success)
        loadImages(imagePaths,images);
    else
        return FAIL;
    
    loadIntrinsicsInto(intrinsics, distortion, intrinsicsFolder);

    success = calibrationFromImageSequence(images,imagePaths,useSuppliedParamters,intrinsics,distortion);
    
//    success = calibrationFromStream();
    
    if(success)
        return 0;
    else
        return FAIL;
}
