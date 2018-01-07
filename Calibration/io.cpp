//
//  io
//  Calibration
//
//  Created by Marc Haubenstock on 06.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#include "io.hpp"

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

bool loadFilesInDirInto(vector<string> &imagePaths,string dirPath){
    
    bool success = true;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirPath.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            
            if(ent->d_name[0] != '.'){
                string imagePath = dirPath + ent->d_name;
                imagePaths.push_back(imagePath);
            }

        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        success = false;
    }
    
    return success;

}

bool loadImages(const vector<string> &imagePaths, vector<Mat>& images){
    
    bool success = true;
    
    for(string imagePath : imagePaths){
        // Read the file
        Mat image = imread(imagePath, CV_LOAD_IMAGE_COLOR);
        
        // Check for invalid input
        if(!image.data )
        {
            cout <<  "Could not open or find the image" << std::endl ;
            success = false;
            break;
        }
        
        images.push_back(image);
        
    }
    

    
    
    return success;
    
}
