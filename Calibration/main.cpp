//
//  main.cpp
//  Calibration
//
//  Created by Marc Haubenstock on 05.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

//https://github.com/IntelRealSense/librealsense/blob/legacy/doc/stepbystep/getting_started_with_openCV.md
// https://github.com/opencv/opencv/blob/16821b877cb8bd406b77d822ea35301a492375b2/samples/cpp/tutorial_code/calib3d/camera_calibration/camera_calibration.cpp

// include the librealsense C++ header file
#include <librealsense/rs.hpp>

// include OpenCV header file
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

float image_width = 640;
float image_height = 480;
float squareSize = 29.0; // in mm
// inner points per row, inner points per column
Size boardSize = Size(7,5);
int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
int calibrationFlags = CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_FIX_ASPECT_RATIO;

int main()
{
    // Create a context object. This object owns the handles to all connected realsense devices
    rs::context ctx;
    
    // Access the first available RealSense device
    rs::device * dev = ctx.get_device(0);
    
    // Configure Infrared stream to run at VGA resolution at 30 frames per second
    dev->enable_stream(rs::stream::color, image_width, image_height, rs::format::bgr8, 30);
    
    //Wrap intrinsic paramters into openCV readable containers
    Mat distCoeffs = Mat::zeros(5, 1, CV_64F);
    Mat cameraMatrix = Mat::zeros(3, 3, CV_64F);
    // Vector for image series
    vector<Mat> rvecs, tvecs;
    // vector of 3D feature points per image
    vector<vector<Point3f>> objectPoints;
    // vector of 2D feature points per image
    vector<vector<Point2f>> imagePoints;
    
    bool capturing = false;
    
    // Get intrinsics of color stream. Can be used to refine later.
    rs_intrinsics intrinsics = dev->get_stream_intrinsics(rs::stream::color);
    cout << "fx:" << intrinsics.fx << " fy:" << intrinsics.fy << "\n";
    cout << "ppx:" << intrinsics.ppx << " ppy:" << intrinsics.ppy << "\n";
    
    cameraMatrix.at<float>(0, 0) = intrinsics.fx;
    cameraMatrix.at<float>(1, 1) = intrinsics.fy;
    cameraMatrix.at<float>(2, 2) = 1.0;
    cameraMatrix.at<float>(0, 2) = intrinsics.ppx;
    cameraMatrix.at<float>(1, 2) = intrinsics.ppy;
    
    cout << "Distortion model:" << intrinsics.model << "\n";
    for (int i = 0; i < 5; i++) {
        float coeff = intrinsics.coeffs[i];
        cout << "Distortion Coeff " << i << ":" << coeff << "\n";
        distCoeffs.at<float>(i,0) = coeff;
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
            calibrateCamera(objectPoints, imagePoints, Size(image_width,image_height), cameraMatrix, distCoeffs, rvecs, tvecs);
            
            cout << "Successfully Calibrated the Camera!\n";
            // Write to File
            
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
    
    //waitKey(0);
    
    return 0;
}
