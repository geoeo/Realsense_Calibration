//
//  writer.hpp
//  Calibration
//
//  Created by Marc Haubenstock on 06.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#ifndef writer_hpp
#define writer_hpp

#include <stdio.h>
#include <fstream>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

bool writeCalibrationDataToDisk(const Mat& intrinsics, const Mat& distortion,const vector<Mat>& rvecs,const vector<Mat>& tvecs, Size imageSize);

#endif /* writer_hpp */
