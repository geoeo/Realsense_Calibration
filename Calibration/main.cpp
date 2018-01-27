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

const int FAIL = 1;

int loadAllImagesFromDir(const string dirPath, vector<string>& imagePaths, vector<Mat>& images){
    
    bool success = loadFilesInDirInto(imagePaths,dirPath);
    if(success)
        loadImages(imagePaths,images);
    else
        return FAIL;
    
    return 0;
    
}

int loadSingleImageFromDir(string imagePath,vector<string>& imagePaths, vector<Mat>& images){
    
    imagePaths.push_back(imagePath);
    
    if(!loadImages(imagePaths,images))
       return FAIL;


    return 0;
    
}


int main()
{

    vector<Mat> images;
    Mat intrinsics = Mat::eye(3, 3, CV_64F);
    Mat distortion = Mat::zeros(5, 1, CV_64F);;
    
    //string intrinsicsFolder = "Calibration_Output/defaults/";
    string intrinsicsFolder = "Calibration_Output/Seq_1/";
    
    vector<string> imagePaths;
    bool success;
    string dirPath;
    string root = returnRoot();
    string imagePath;

    imagePath.append(root).append("Image_Seq_1/").append("image_52.png");
    dirPath.append(root).append("Image_Seq_1/");
    
    // change this to calibrate the camera on a given image set
    bool useSuppliedParamters = true;

//    //Used for calibration
//    if(loadAllImagesFromDir(dirPath,imagePaths,images) == FAIL)
//        return FAIL;
    
    // Used to estimate camera extrinsics for single image
    if(loadSingleImageFromDir(imagePath,imagePaths, images) == FAIL)
        return FAIL;
    
    loadIntrinsicsInto(intrinsics, distortion, intrinsicsFolder);

    success = calibrationFromImageSequence(images,imagePaths,useSuppliedParamters,intrinsics,distortion);
    
//    success = calibrationFromStream();
    
    if(success)
        return 0;
    else
        return FAIL;
}
