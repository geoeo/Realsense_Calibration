//
//  calibration.cpp
//  Calibration
//
//  Created by Marc Haubenstock on 07.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

//https://github.com/IntelRealSense/librealsense/blob/legacy/doc/stepbystep/getting_started_with_openCV.md
// https://github.com/opencv/opencv/blob/16821b877cb8bd406b77d822ea35301a492375b2/samples/cpp/tutorial_code/calib3d/camera_calibration/camera_calibration.cpp

#include "calibration.hpp"

using namespace std;
using namespace cv;

float image_width = 640;
float image_height = 480;
float squareSize = 29.0; // in mm
//float squareSize = 2.9; // in cm
//float squareSize = 0.0029; // in meters
//float squareSize = 1.0; // in mm
// inner points per row, inner points per column
Size boardSize = Size(7,5);
Size imageSize = Size(image_width,image_height);
int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
int calibrationFlags = CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_FIX_ASPECT_RATIO; // | CV_CALIB_FIX_FOCAL_LENGTH;
float rms = 0;

bool calibrationFromStream(){
    bool success = true;
    
    // Create a context object. This object owns the handles to all connected realsense devices
    rs::context ctx;
    
    // Access the first available RealSense device
    rs::device * dev = ctx.get_device(0);
    
    // Configure Infrared stream to run at VGA resolution at 30 frames per second
    dev->enable_stream(rs::stream::color, image_width, image_height, rs::format::bgr8, 30);
    
    //Wrap intrinsic paramters into openCV readable containers
    Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    //Mat cameraMatrix = Mat::zeros(3, 3, CV_32F);
    // Vector for image series
    vector<Mat> rvecs, tvecs,tvecs_no_rot;
    // vector of 3D feature points per image
    vector<vector<Point3f>> objectPoints;
    // vector of 2D feature points per image
    vector<vector<Point2f>> imagePoints;
    // dummy vector
    vector<string> imagePaths;
    
    bool capturing = false;
    
    // Get intrinsics of color stream. Can be used to refine later.
    rs_intrinsics intrinsics = dev->get_stream_intrinsics(rs::stream::color);
    cout << "fx:" << intrinsics.fx << " fy:" << intrinsics.fy << "\n";
    cout << "ppx:" << intrinsics.ppx << " ppy:" << intrinsics.ppy << "\n";
    
    cameraMatrix.at<double>(0, 0) = intrinsics.fx; // 610.367
    cameraMatrix.at<double>(1, 1) = intrinsics.fy; // 615.825
    cameraMatrix.at<double>(0, 2) = intrinsics.ppx; // 332.213
    cameraMatrix.at<double>(1, 2) = intrinsics.ppy; // 225.817
    
//    Distortion Coeff 0:-0.0658501
//    Distortion Coeff 1:0.0744128
//    Distortion Coeff 2:-0.000177425
//    Distortion Coeff 3:-0.00126745
//    Distortion Coeff 4:0
    cout << "Distortion model:" << intrinsics.model << "\n";
    for (int i = 0; i < 5; i++) {
        double coeff = intrinsics.coeffs[i];
        cout << "Distortion Coeff " << i << ":" << coeff << "\n";
        distCoeffs.at<double>(i,0) = coeff;
    }
    
    
    // vector of 2D features
    vector<Point2f> pointBuf;
    
    // vector of features in 3D space
    vector<Point3f> corners;
    // Since location of corners wont change in 3D (assuming a static map) we can precompute them
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
        }
    }
    
    // Start streaming
    dev->start();
    
    // Camera warmup - Dropped several first frames to let auto-exposure stabilize
    for(int i = 0; i < 30; i++)
        dev->wait_for_frames();
    
    // Display in a GUI
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    
    while(true){
        
        // Creating OpenCV Matrix from a color image
        Mat view(Size(image_width, image_height), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        
        char keyPressed = (char)waitKey(1);
        //cout << "Key pressed: " << (int)keyPressed << "\n";
        
        // Enter
        if(keyPressed == 13){
            dev->stop();
            ctx.~context();
            
            cout << "Enter pressed..\n";
            cout << "Processing Image Sections for Calibration..\n";
            capturing = false;
            
            // duplicate the object points (3D) for every 2D feature space
            objectPoints.assign(imagePoints.size(), corners);
            
            // run calibration over feature sequences
            rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,calibrationFlags);
            
            removeRotationFromTranslation(rvecs,tvecs,tvecs_no_rot);
            
            cout << "Successfully Calibrated the Camera!\n";
            cout << "RMS: " << rms << std::endl;
            // Write to File
            writeCalibrationDataToDisk(cameraMatrix, distCoeffs, rvecs, tvecs,tvecs_no_rot, imageSize,imagePaths,rms);
            
            break;
            
        }
        
        // ESC pressed
        else if(keyPressed == 27){
            cout << "Exiting..\n";
            dev->stop();
            ctx.~context();
            break;
        }
        
        // c pressed
        else if(capturing || keyPressed == 'c'){
            
            if(keyPressed == 'c')
                capturing = !capturing;
            
            bool found = findChessboardCorners( view, boardSize, pointBuf, chessBoardFlags);
            
            if(found){
                Mat viewGray;
                // Convert image to grey scale
                cvtColor(view, viewGray, COLOR_BGR2GRAY);
                // Use sub pixel interpolation to find more accurate corener estimates
                cornerSubPix( viewGray, pointBuf, Size(11,11),
                             Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
                // draw corners (Pointbuf) into image
                drawChessboardCorners( view, boardSize, Mat(pointBuf), found );
                imshow("Display Image", view);
                
                imagePoints.push_back(pointBuf);
                cout << "Images Captued: "<< imagePoints.size() << "\n";
                
            }
            else {
                imshow("Display Image", view);
                capturing = false;
            }
            
        }
        
        else{
            imshow("Display Image", view);
        }
        
        dev->wait_for_frames();
        
    }

    return success;
}

bool calibrationFromImageSequence(const vector<Mat>& images, const vector<string>& imagePaths,bool useCameraDefaults){
    bool success = true;
    
    //Wrap intrinsic paramters into openCV readable containers
    Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    //Mat cameraMatrix = Mat::zeros(3, 3, CV_32F);
    // Vector for image series
    vector<Mat> rvecs, tvecs, tvecs_no_rot;
    // vector of 3D feature points per image
    vector<vector<Point3f>> objectPoints;
    // vector of 2D feature points per image
    vector<vector<Point2f>> imagePoints;
    float totalError = 0;
    
    // defaults from camera
    float fx = 610.367; float fy = 615.825;
    float ppx = 332.213; float ppy = 225.817;

    cameraMatrix.at<double>(0, 0) = fx;
    cameraMatrix.at<double>(1, 1) = fy;
    cameraMatrix.at<double>(0, 2) = ppx;
    cameraMatrix.at<double>(1, 2) = ppy;
    
    distCoeffs.at<double>(0,0) = -0.0658501;
    distCoeffs.at<double>(1,0) = 0.0744128;
    distCoeffs.at<double>(2,0) = -0.000177425;
    distCoeffs.at<double>(3,0) = -0.00126745;
    distCoeffs.at<double>(4,0) = 0;

    // vector of 2D features
    vector<Point2f> pointBuf;
    
    // vector of features in 3D space
    vector<Point3f> corners;
    // Since location of corners wont change in 3D (assuming a static map) we can precompute them
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
        }
    }
    
    // Create a window for display.
    //namedWindow( "Display window", WINDOW_AUTOSIZE );
    
    cout << "Starting Corner Detect" << endl;
    
    for(Mat image : images){

        bool found = findChessboardCorners( image, boardSize, pointBuf, chessBoardFlags);
        
        if(found){
            Mat viewGray;
            // Convert image to grey scale
            cvtColor(image, viewGray, COLOR_BGR2GRAY);
            // Use sub pixel interpolation to find more accurate corener estimates
            cornerSubPix( viewGray, pointBuf, Size(11,11),
                         Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
            // draw corners (Pointbuf) into image
            //drawChessboardCorners( image, boardSize, Mat(pointBuf), found );
            //imshow("Display Image", image);
            
            imagePoints.push_back(pointBuf);
            
        }
        
    }
    
    cout << "Found Corners for: "<< imagePoints.size()  << " images" << endl;
    
    // duplicate the object points (3D) for every 2D feature space
    objectPoints.assign(imagePoints.size(), corners);
    
    if(!useCameraDefaults){
        // run calibration over feature sequences
        rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,calibrationFlags);
    }
    else{
        rms = 0;
        for (int i = 0; i < objectPoints.size(); i++) {
            Mat rvec,tvec;
            success = solvePnP(objectPoints[i], imagePoints[i], cameraMatrix, distCoeffs, rvec, tvec, false,SOLVEPNP_ITERATIVE);
            if(!success)
                return success;
            rvecs.push_back(rvec);
            tvecs.push_back(tvec);
            vector<Point2f> imagePointsProjected;
            projectPoints(objectPoints[i], rvec, tvec, cameraMatrix, distCoeffs, imagePointsProjected);
            totalError += norm(imagePoints[i], imagePointsProjected,NORM_L2);
        }
        rms = sqrt(totalError/objectPoints.size());
    }

    
    removeRotationFromTranslation(rvecs,tvecs,tvecs_no_rot);
    
    cout << "Successfully Calibrated the Camera!\n";
    cout << "RMS: " << rms << std::endl;
    // Write to File
    writeCalibrationDataToDisk(cameraMatrix, distCoeffs, rvecs, tvecs,tvecs_no_rot,imageSize,imagePaths, rms);
    
    
    
    return success;
}

// remove the rotation component from the translation
// https://stackoverflow.com/questions/45458665/get-correct-rvec-and-tvec-for-camera-pose-estimation-from-solvpnp-function-in-op?rq=1
void removeRotationFromTranslation(const vector<Mat>& rvecs,const vector<Mat>& tvecs,vector<Mat>& tvecs_no_rotation){
    

    for (int i = 0; i < rvecs.size(); i++) {
        Mat rotation_front;
        Mat world_position_front_cam;
        
        //Get rotation matrix
        Rodrigues(rvecs[i], rotation_front);
        
        //Get rotation matrix inverse
        Mat rotation_inverse;
        transpose(rotation_front, rotation_inverse);
        
        //Get camera position in world cordinates
        world_position_front_cam = -rotation_inverse * tvecs[i];
        tvecs_no_rotation.push_back(world_position_front_cam);
        
        //cout << "x: " << world_position_front_cam.at<double>(0,0) << " y:" << world_position_front_cam.at<double>(1,0) << " z: " << world_position_front_cam.at<double>(2,0) << endl;
    }
    
}
