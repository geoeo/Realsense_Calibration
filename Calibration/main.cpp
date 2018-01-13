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
#include "io.hpp"


int main()
{
    const int FAIL = 1;
    vector<Mat> images;
    vector<string> imagePaths;
    //string dirPath = "/Users/marchaubenstock/Workspace/Xcode/Calibration/Images/";
    string dirPath = "/Users/marchaubenstock/Workspace/Xcode/Calibration/Image_Seq_2/";
    bool success = true;

    success = loadFilesInDirInto(imagePaths,dirPath);
    if(success)
        loadImages(imagePaths,images);
    else
        return FAIL;

    success = calibrationFromImageSequence(images,imagePaths);
    
//    success = calibrationFromStream();
    
    if(success)
        return 0;
    else
        return FAIL;
}
