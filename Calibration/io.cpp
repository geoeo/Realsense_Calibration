//
//  io
//  Calibration
//
//  Created by Marc Haubenstock on 06.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#include "io.hpp"

bool writeCalibrationDataToDisk(const Mat& intrinsics, const Mat& distortion,const vector<Mat>& rvecs,const vector<Mat>& tvecs,vector<Mat>& tvecs_no_rot, Size imageSize, const vector<string>& imagePaths,float rms){
    bool success = true;
    
    fstream fs;
    fs.open("intrinsics.txt", fstream::in | fstream::out | fstream::trunc);
    fs << intrinsics.at<double>(0, 0) << endl; // fx
    fs << intrinsics.at<double>(1, 1) << endl; // fy
    fs << intrinsics.at<double>(0, 2) << endl; // ppx
    fs << intrinsics.at<double>(1, 2) << endl; // ppy
    fs.close();
    
    fs.open("distortion.txt", fstream::in | fstream::out | fstream::trunc);
    fs << distortion.at<double>(0,0) << endl; // k1
    fs << distortion.at<double>(1,0) << endl; // k2
    fs << distortion.at<double>(2,0) << endl; // p1
    fs << distortion.at<double>(3,0) << endl; // p2
    fs << distortion.at<double>(4,0) << endl; // k3
    fs.close();
    
    fs.open("dimensions.txt", fstream::in | fstream::out | fstream::trunc);
    fs << imageSize.width << endl; // width
    fs << imageSize.height << endl; // height
    fs << rms << endl; // rms
    fs.close();
    
    fs.open("tvec.txt", fstream::in | fstream::out | fstream::trunc);
    // 3x1: x,y,z
    for(Mat tvec : tvecs){
         fs << tvec.at<double>(0, 0) << "," << tvec.at<double>(1, 0) << "," << tvec.at<double>(2, 0) << endl;
    }
    fs.close();
    
    fs.open("tvec_no_rot.txt", fstream::in | fstream::out | fstream::trunc);
    // 3x1: x,y,z
    for(Mat tvec : tvecs_no_rot){
        fs << tvec.at<double>(0, 0) << "," << tvec.at<double>(1, 0) << "," << tvec.at<double>(2, 0) << endl;
    }
    fs.close();
    
    fs.open("rvec.txt", fstream::in | fstream::out | fstream::trunc);
    // 3x1: Rodriguiz format
    for(Mat rvec : rvecs){
        fs << rvec.at<double>(0, 0) << "," << rvec.at<double>(1, 0) << "," << rvec.at<double>(2, 0) << endl;
    }
    fs.close();
    
    fs.open("imageOrder.txt", fstream::in | fstream::out | fstream::trunc);
    for(string path : imagePaths){
        fs << path << endl;
    }
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
