#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
using namespace cv;
using namespace std;

Mat redFilter(const Mat& src)
{
  assert(src.type() == CV_8UC3);

  Mat redOnly;
  inRange(src, Scalar(0, 0, 0), Scalar(0, 0, 255), redOnly);

  return redOnly;
}


int main(int argc, char** argv)
{
  Mat input = imread(argv[1]);
  int posX=669, posY=429;
  circle(input , Point(posX, posY), 3 , Scalar( 252, 22, 120 ), -1, 8);
  imshow("redOnly", input);
  waitKey();
  return 0; 
}


int main_2(int argc, char** argv)
{
  Mat input = imread(argv[1]);
  Mat imgHSV;
  cvtColor(input, imgHSV, COLOR_BGR2HSV);
  int hh = 0, hl = 0, sh = 0, sl = 0, vh = 0, vl = 255;

  string windowName = "background";
  namedWindow(windowName);

  createTrackbar("hh", windowName, &hh, 255);
  createTrackbar("hl", windowName, &hl, 255);
  createTrackbar("sh", windowName, &sh, 255);
  createTrackbar("sl", windowName, &sl, 255);
  createTrackbar("vh", windowName, &vh, 255);
  createTrackbar("vl", windowName, &vl, 255);
  //Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
  Mat bgIsolation;
  int key = 0;
  do
  {
    inRange(imgHSV, Scalar(hl, sl, vl), Scalar(hh, sh, vh), bgIsolation);

    //bitwise_not(bgIsolation, bgIsolation);
    //erode(bgIsolation, bgIsolation, Mat());
    //dilate(bgIsolation, bgIsolation, element);

    imshow(windowName, bgIsolation);
    key = waitKey(33);
  } while((char)key != 27);

  waitKey();

  return 0;
}

void showMarker(Mat &image){
  Moments oMoments = moments(image);
  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;
  int posX, posY;
  if(dArea > 0)
  {
    posX = dM10 / dArea;
    posY = dM01 / dArea;
    //circle(image, Point(posX, posY), 3 , Scalar( 252, 22, 120 ), -1, 8);
    cout << "coord:" << posX <<","<<posY << "\n";
  }
}
int main__s(int argc, char** argv)
{
  Mat src = imread(argv[1]);


  int rh = 255, rl = 100, gh = 255, gl = 0, bh = 70, bl = 0;

  string windowName = "background";
  namedWindow(windowName);

  createTrackbar("rh", windowName, &rh, 255);
  createTrackbar("rl", windowName, &rl, 255);
  createTrackbar("gh", windowName, &gh, 255);
  createTrackbar("gl", windowName, &gl, 255);
  createTrackbar("bh", windowName, &bh, 255);
  createTrackbar("bl", windowName, &bl, 255);

  //for dilation
  //Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));

  Mat bgIsolation;
  int key = 0;
  //do
  //{

    double a = getTickCount();
    inRange(src, Scalar(bl, gl, rl), Scalar(bh, gh, rh), bgIsolation);

    //bitwise_not(bgIsolation, bgIsolation);

    //erode(bgIsolation, bgIsolation, Mat());
    //dilate(bgIsolation, bgIsolation, element);
    showMarker(bgIsolation);
    double b = getTickCount();
    cout << "Time taken: " <<(b-a)/getTickFrequency() << "\n";
    imshow(windowName, bgIsolation);
    key = waitKey(33);
  //} while((char)key != 27);

  waitKey();

  return 0;
}

