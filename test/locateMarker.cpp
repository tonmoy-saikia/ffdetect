#include<stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, const char** argv)
{
  Mat image;
  vector<Mat> rgbChannels(3);

  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  split( image, rgbChannels);
  double m, M;
  Point p_min, p_max;
  minMaxLoc(rgbChannels[2], &m, &M, &p_min, &p_max);
  cout<< p_max;
  circle(image,p_max,5,Scalar(252, 22, 120));
  Mat g, fin_img;
  g = Mat::zeros(Size(image.cols, image.rows), CV_8UC1);
  vector<Mat> channels;
  channels.push_back(g);
  channels.push_back(g);
  channels.push_back(rgbChannels[2]);

  /// Merge the three channels
  merge(channels, fin_img);
  namedWindow("R",1);imshow("R", fin_img);

  //namedWindow( "Display window", WINDOW_AUTOSIZE );
  imshow( "Display window", image );
  waitKey(50000);
  return 0;
}
