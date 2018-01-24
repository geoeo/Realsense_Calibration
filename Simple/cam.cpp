#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

int main(){
   VideoCapture cap(0);
   Mat new_frame;
   while(true){
      cap >> new_frame;
      if(new_frame.empty()){
         break;
      }
      imshow("camera", new_frame);
      // press ESC to quit software
      if( (int)waitKey(10) == 27 ){
         break;
      }
   }
}