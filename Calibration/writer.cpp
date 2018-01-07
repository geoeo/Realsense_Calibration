//
//  writer.cpp
//  Calibration
//
//  Created by Marc Haubenstock on 06.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#include "writer.hpp"

bool writeCalibrationDataToDisk(const Mat& intrinsics, const Mat& distortion,const vector<Mat>& rvecs,const vector<Mat>& tvecs, Size imageSize){
    bool success = true;
    
    fstream fs;
    fs.open("intrinsics.txt", fstream::in | fstream::out | fstream::trunc);
    fs << intrinsics.at<double>(0, 0) << "\n"; // fx
    fs << intrinsics.at<double>(1, 1) << "\n"; // fy
    fs << intrinsics.at<double>(0, 2) << "\n"; // ppx
    fs << intrinsics.at<double>(1, 2) << "\n"; // ppy
    fs.close();
    
    fs.open("distortion.txt", fstream::in | fstream::out | fstream::trunc);
    fs << distortion.at<double>(0,0) << "\n"; // k1
    fs << distortion.at<double>(1,0) << "\n"; // k2
    fs << distortion.at<double>(2,0) << "\n"; // p1
    fs << distortion.at<double>(3,0) << "\n"; // p2
    fs << distortion.at<double>(4,0) << "\n"; // k3
    fs.close();
    
    fs.open("dimensions.txt", fstream::in | fstream::out | fstream::trunc);
    fs << imageSize.width << "\n"; // width
    fs << imageSize.height << "\n"; // height
    fs.close();
    
    //TODO WRITE ALL
    Mat tvec = tvecs[0]; // 3x1
    fs.open("tvec.txt", fstream::in | fstream::out | fstream::trunc);
    fs << tvec.at<float>(0, 0) << "\n";
    fs << tvec.at<float>(1, 0) << "\n";
    fs << tvec.at<float>(2, 0) << "\n";
    fs.close();
    
    return success;
    
}
