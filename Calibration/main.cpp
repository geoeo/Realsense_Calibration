//
//  main.cpp
//  Calibration
//
//  Created by Marc Haubenstock on 05.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

//https://github.com/IntelRealSense/librealsense/blob/legacy/doc/stepbystep/getting_started_with_openCV.md

// include the librealsense C++ header file
#include <librealsense/rs.hpp>

// include OpenCV header file
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

int main()
{
    // Create a context object. This object owns the handles to all connected realsense devices
    rs::context ctx;
    
    // Access the first available RealSense device
    rs::device * dev = ctx.get_device(0);
    
    // Configure Infrared stream to run at VGA resolution at 30 frames per second
    dev->enable_stream(rs::stream::color, 640, 480, rs::format::bgr8, 30);
    
    // Start streaming
    dev->start();
    
    // vector of 2D features
    vector<Point2f> pointBuf;
    
    // inner points per row, inner points per column
    Size boardSize = Size(7,5);
    
    // Camera warmup - Dropped several first frames to let auto-exposure stabilize
    for(int i = 0; i < 30; i++)
        dev->wait_for_frames();
    
    // Display in a GUI
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    
    while(true){
        
        // Creating OpenCV Matrix from a color image
        Mat view(Size(640, 480), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        
        //imshow("Display Image", view);
        
        int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
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
        }
        else {
            imshow("Display Image", view);
        }
        
        
        // ESC pressed
        if((char)waitKey(1) == 27){
            dev->stop();
            ctx.~context();
            break;
        }
        
        dev->wait_for_frames();
        
    }
    
    //waitKey(0);
    
    return 0;
}
