//
//  main.cpp
//  ImageCreation
//
//  Created by Marc Haubenstock on 13.01.18.
//  Copyright © 2018 Marc Haubenstock. All rights reserved.
//

#include <iostream>

// include the librealsense C++ header file
#include <librealsense/rs.hpp>

// include OpenCV header file
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

float image_width = 640;
float image_height = 480;
Size boardSize = Size(7,5);
bool capturing = false;
string dirPath = "/Users/marchaubenstock/Workspace/Xcode/Calibration/Images/";
int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
int imageCounter = 1;

int main(int argc, const char * argv[]) {
    // insert code here...
    // Create a context object. This object owns the handles to all connected realsense devices
    rs::context ctx;
    
    // Access the first available RealSense device
    rs::device * dev = ctx.get_device(0);
    
    // Configure Infrared stream to run at VGA resolution at 30 frames per second
    dev->enable_stream(rs::stream::color, image_width, image_height, rs::format::bgr8, 30);
    
    // Start streaming
    dev->start();
    
    // Camera warmup - Dropped several first frames to let auto-exposure stabilize
    for(int i = 0; i < 30; i++)
        dev->wait_for_frames();
    
    // Display in a GUI
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    
    // vector of 2D features
    vector<Point2f> pointBuf;
    
    // compression flags
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    
    while(true){
        
        // Creating OpenCV Matrix from a color image
        Mat view(Size(image_width, image_height), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        
        char keyPressed = (char)waitKey(1);
        //cout << "Key pressed: " << (int)keyPressed << "\n";
        
        // ESC pressed
        if(keyPressed == 27){
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
                drawChessboardCorners( viewGray, boardSize, Mat(pointBuf), found );
                imshow("Display Image", viewGray);
                
                // Enter
                //if(keyPressed == 13){
                    try {
                        string imageName(dirPath + "image_" + std::to_string(imageCounter)+".png");
                        imwrite(imageName, view, compression_params);
                        cout << imageName << " saved to disk" << std::endl;
                        imageCounter++;
                    }
                    catch (runtime_error& ex) {
                        fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
                        return 1;
                    }
                //}
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
    
    return 0;
}
