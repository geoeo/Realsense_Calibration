//
//  io
//  Calibration
//
//  Created by Marc Haubenstock on 06.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#include "io.hpp"

void writeCalibrationDataToDisk(const Mat& intrinsics, const Mat& distortion,const vector<Mat>& rvecs,const vector<Mat>& tvecs,vector<Mat>& tvecs_no_rot, Size imageSize, const vector<string>& imagePaths,float rms){

    string root = returnRoot();
    string intrinsicFilePath;
    string distortionFilePath;
    string dimensionFilePath;
    string tvecFilePath;
    string tvec_no_rotFilePath;
    string rvecFilePath;
    string imageOrderFilePath;

    intrinsicFilePath.append(root).append("intrinsics.txt");
    distortionFilePath.append(root).append("distortion.txt");
    dimensionFilePath.append(root).append("dimension.txt");
    tvecFilePath.append(root).append("tvec.txt");
    tvec_no_rotFilePath.append(root).append("tvec_no_rot.txt");
    rvecFilePath.append(root).append("rvec.txt");
    imageOrderFilePath.append(root).append("imageOrder.txt");

    fstream fs;
    fs.open(intrinsicFilePath, fstream::in | fstream::out | fstream::trunc);
    fs << intrinsics.at<double>(0, 0) << endl; // fx
    fs << intrinsics.at<double>(1, 1) << endl; // fy
    fs << intrinsics.at<double>(0, 2) << endl; // ppx
    fs << intrinsics.at<double>(1, 2) << endl; // ppy
    fs.close();
    
    fs.open(distortionFilePath, fstream::in | fstream::out | fstream::trunc);
    fs << distortion.at<double>(0,0) << endl; // k1
    fs << distortion.at<double>(1,0) << endl; // k2
    fs << distortion.at<double>(2,0) << endl; // p1
    fs << distortion.at<double>(3,0) << endl; // p2
    fs << distortion.at<double>(4,0) << endl; // k3
    fs.close();
    
    fs.open(dimensionFilePath, fstream::in | fstream::out | fstream::trunc);
    fs << imageSize.width << endl; // width
    fs << imageSize.height << endl; // height
    fs << rms << endl; // rms
    fs.close();
    
    fs.open(tvecFilePath, fstream::in | fstream::out | fstream::trunc);
    // 3x1: x,y,z
    for(Mat tvec : tvecs){
         fs << tvec.at<double>(0, 0) << "," << tvec.at<double>(1, 0) << "," << tvec.at<double>(2, 0) << endl;
    }
    fs.close();
    
    fs.open(tvec_no_rotFilePath, fstream::in | fstream::out | fstream::trunc);
    // 3x1: x,y,z
    for(Mat tvec : tvecs_no_rot){
        fs << tvec.at<double>(0, 0) << "," << tvec.at<double>(1, 0) << "," << tvec.at<double>(2, 0) << endl;
    }
    fs.close();
    
    fs.open(rvecFilePath, fstream::in | fstream::out | fstream::trunc);
    // 3x1: Rodriguiz format, https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    for(Mat rvec : rvecs){
        fs << rvec.at<double>(0, 0) << "," << rvec.at<double>(1, 0) << "," << rvec.at<double>(2, 0) << endl;
    }
    fs.close();
    
    fs.open(imageOrderFilePath, fstream::in | fstream::out | fstream::trunc);
    for(string path : imagePaths){
        fs << path << endl;
    }
    fs.close();

    
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

bool loadImages(const vector<string> &imagePaths, vector<Mat>& images, int offset){
    
    bool success = true;
    
    //for(string imagePath : imagePaths){
    for(int i = 0; i < imagePaths.size(); i+=offset){
        string imagePath = imagePaths[i];
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

void loadIntrinsicsInto(Mat& intrinsics, Mat& distortion, const string folder){
    
    string root = returnRoot();
    string intrinsicFilePath;
    string distortionFilePath;
    char lineRaw[255];
    vector<double> intrinsicsVector; // fx,fy, ppx, ppy
    vector<double> distortionVector; // k1, k2, p1, p2, k3.
    
    intrinsicFilePath.append(root).append(folder).append("intrinsics.txt");
    distortionFilePath.append(root).append(folder).append("distortion.txt");
    
    fstream fs;
    
    fs.open(intrinsicFilePath, fstream::in);
    cout << "Reading Intrinsics from File.." << endl;
    
    while(fs){
        fs.getline(lineRaw, 255);
        string line(lineRaw);
        if(line.empty())
            break;
        cout << line << endl;
        double val = atof(lineRaw);
        intrinsicsVector.push_back(val);
        
    }
    fs.close();
    
    fs.open(distortionFilePath, fstream::in);
    cout << "Reading Distortion Coeffs from File.." << endl;
    
    while(fs){
        fs.getline(lineRaw, 255);
        string line(lineRaw);
        if(line.empty())
            break;
        cout << line << endl;
        double val = atof(lineRaw);
        distortionVector.push_back(val);
    }
    fs.close();
    
    intrinsics.at<double>(0,0) = intrinsicsVector.at(0);
    intrinsics.at<double>(1,1) = intrinsicsVector.at(1);
    intrinsics.at<double>(0,2) = intrinsicsVector.at(2);
    intrinsics.at<double>(1,2) = intrinsicsVector.at(3);
    
    distortion.at<double>(0,0) = distortionVector.at(0);
    distortion.at<double>(1,0) = distortionVector.at(1);
    distortion.at<double>(2,0) = distortionVector.at(2);
    distortion.at<double>(3,0) = distortionVector.at(3);
    distortion.at<double>(4,0) = distortionVector.at(4);
    
    
    
}
