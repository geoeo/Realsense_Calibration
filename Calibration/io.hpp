//
//  io.hpp
//  Calibration
//
//  Created by Marc Haubenstock on 06.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#ifndef io_hpp
#define io_hpp

#include <stdio.h>
#include <fstream>
#include <ctime>
#include <dirent.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

bool writeCalibrationDataToDisk(const Mat& intrinsics, const Mat& distortion,const vector<Mat>& rvecs,const vector<Mat>& tvecs,vector<Mat>& tvecs_no_rot, Size imageSize, const vector<string>& imagePaths, float rms);

bool loadImages(const vector<string> &imagePaths,vector<Mat>& images);

bool loadFilesInDirInto(vector<string> &imagePaths,string dirPath);

#endif /* io_hpp */
