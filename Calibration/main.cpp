//
//  main.cpp
//  Calibration
//
//  Created by Marc Haubenstock on 05.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>
//
//
//using namespace std;
//using namespace cv;
//
//int main(){
//
//
//    VideoCapture cap(0);
//    Mat new_frame;
//    while(true){
//        cap >> new_frame;
//        if(new_frame.empty()){
//            break;
//        }
//        imshow("camera", new_frame);
//        // press ESC to quit software
//        if( (int)waitKey(10) == 27 ){
//            break;
//        }
//    }
//}

//https://github.com/IntelRealSense/librealsense/blob/legacy/doc/stepbystep/getting_started_with_openCV.md

// include the librealsense C++ header file
#include <librealsense/rs.hpp>

// include OpenCV header file
#include <opencv2/opencv.hpp>

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
    
    // Camera warmup - Dropped several first frames to let auto-exposure stabilize
    for(int i = 0; i < 30; i++)
        dev->wait_for_frames();
    
    // Display in a GUI
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    
    while(true){
        
        // Creating OpenCV Matrix from a color image
        Mat color(Size(640, 480), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        
        imshow("Display Image", color);
        dev->wait_for_frames();
        
        if((char)waitKey(1) == 27){
            // ESC pressed
            break;
        }
        
    }
    
    //waitKey(0);
    
    return 0;
}
