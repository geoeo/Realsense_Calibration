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
//string dirPath = "/Users/marchaubenstock/Workspace/Xcode/Calibration/Images/";
string dirPath = "/Users/marchaubenstock/Workspace/Xcode/Calibration/Images_ZR300/";
int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
int imageCounter = 1;
bool detectCorners = false; // flag to determine if conrers should be detected or all images should be saved
string depthScaleFilePath;
fstream fs;

rs::stream colorStream = rs::stream::color;
rs::stream depthStream = rs::stream::depth;
rs::stream depthAlignedToRectifiedColorStream = rs::stream::depth_aligned_to_rectified_color;
rs::stream depthAlignedTColorStream = rs::stream::depth_aligned_to_color;
rs::stream colorRectifiedStream = rs::stream::rectified_color;


int main(int argc, const char * argv[]) {
    // insert code here...
    // Create a context object. This object owns the handles to all connected realsense devices
    rs::context ctx;
    
    // Access the first available RealSense device
    rs::device * dev = ctx.get_device(0);
    
    // Configure Infrared stream to run at VGA resolution at 30 frames per second
    //dev->enable_stream(rs::stream::color, image_width, image_height, rs::format::bgr8, 30);
    dev->enable_stream(colorStream, image_width, image_height, rs::format::bgr8, 30);
    dev->enable_stream(depthStream, image_width, image_height, rs::format::z16, 30);
    
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
    
    depthScaleFilePath.append(dirPath).append("depthScale.txt");
    fs.open(depthScaleFilePath, fstream::in | fstream::out | fstream::trunc);
    
    while(true){
        
        // Creating OpenCV Matrix from a color image
        Mat view(Size(image_width, image_height), CV_8UC3, (void*)dev->get_frame_data(colorStream), Mat::AUTO_STEP);
        Mat viewDepthAligned(Size(image_width, image_height), CV_16U, (void*)dev->get_frame_data(depthAlignedTColorStream), Mat::AUTO_STEP);
        //Mat viewDepth(Size(image_width, image_height), CV_16U, (void*)dev->get_frame_data(depthStream), Mat::AUTO_STEP);
        float depth_scale = dev->get_depth_scale();
        
        viewDepthAligned = viewDepthAligned * depth_scale;
        //viewDepth = viewDepth * depth_scale;
        
        //rs::extrinsics extrinsics = dev->get_extrinsics(depthStream, colorStream);
        
        char keyPressed = (char)waitKey(1);
        //cout << "Key pressed: " << (int)keyPressed << "\n";
        
        // ESC pressed
        if(keyPressed == 27){
            cout << "Exiting..\n";
            dev->stop();
            ctx.~context();
            fs.close();
            break;
        }
        
        // c pressed
        else if(capturing || keyPressed == 'c'){
            
            if(keyPressed == 'c')
                capturing = !capturing;
            
            bool found = detectCorners ? findChessboardCorners( view, boardSize, pointBuf, chessBoardFlags) : true;
            
            if(found){
                if(detectCorners){
                    Mat viewGray;
                    // Convert image to grey scale
                    cvtColor(view, viewGray, COLOR_BGR2GRAY);
                    // Use sub pixel interpolation to find more accurate corener estimates
                    cornerSubPix( viewGray, pointBuf, Size(11,11),
                                 Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
                    // draw corners (Pointbuf) into image
                    drawChessboardCorners( viewGray, boardSize, Mat(pointBuf), found );
                    imshow("Display Image", view);
                    //imshow("Display Image Depth", viewDepth);
                    //imshow("Display Image Depth Aligned", viewDepthAligned);
                }
                else{
                        imshow("Display Image", view);
                        //imshow("Display Image Depth", viewDepth);
                        //imshow("Display Image Depth Aligned", viewDepthAligned);
                }

                
                try {
                    string imageCounterString = std::to_string(imageCounter);
                    string imageName(dirPath + "image_" + imageCounterString +".png");
                    string imageNameDepth(dirPath + "image_depth_" + imageCounterString +".png");
                    string imageNameDepthAligned(dirPath + "image_depth_aligned_" + imageCounterString +".png");
                    
                    imwrite(imageName, view, compression_params);
                    //imwrite(imageNameDepth, viewDepth, compression_params);
                    imwrite(imageNameDepthAligned, viewDepthAligned, compression_params);

                    fs << depth_scale << endl;
                    cout << imageName << " saved to disk" << std::endl;
                    imageCounter++;
                }
                catch (runtime_error& ex) {
                    fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
                    dev->stop();
                    ctx.~context();
                    fs.close();
                    return 1;
                }
            }
            else {
                imshow("Display Image", view);
                //imshow("Display Image Depth", viewDepth);
                imshow("Display Image Depth Aligned", viewDepthAligned);
                capturing = false;
            }
            
        }
        
        else{
            imshow("Display Image", view);
            //imshow("Display Image Depth", viewDepth);
            imshow("Display Image Depth Aligned", viewDepthAligned);
        }
        
        dev->wait_for_frames();
        
    }
    
    return 0;
}
