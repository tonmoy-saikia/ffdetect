#include<stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, const char** argv)
{
  //VideoCapture cap(0);  
  Mat frame;
  VideoCapture videoSource;
  if(!videoSource.open(argv[1])){
    exit(-1);
  }
  int emptyCount = 0;
  videoSource.set(CV_CAP_PROP_CONVERT_RGB, 1);
  int frameCnt = videoSource.get(CV_CAP_PROP_FRAME_COUNT);
  std::cout<< frameCnt;
  vector<Mat> frames;
  for(int i=1; i <= frameCnt; i++)
  {
    videoSource >> frame;
    if(frame.empty()){
      //emptyCount++;
      break;
    }
    frames.push_back(frame);
    //imshow("output", frame);
  }
  imshow("output", frames.back());
  waitKey(10000);
  cout<< "\n" << "empty" << emptyCount <<"\n";
  cout <<"frame cnt:" << (frameCnt - emptyCount);
  return 0;
}
