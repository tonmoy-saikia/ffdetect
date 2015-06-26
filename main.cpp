#include "includes/common.h"
#include "includes/FaceFeatures.h"

#include <fstream>

void extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect);
cv::Point locateMarker(cv::Mat clrImg);


int main(int argc, const char** argv)
{
  config::load_config_file();
  cv::CascadeClassifier c;
  //c.load("/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_frontalface_alt2.xml");
  c.load(config::HAARCASCADE_FACE);
  cv::Mat image, grayImage;
  image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  cvtColor(image, grayImage, CV_BGR2GRAY);
  equalizeHist(grayImage, grayImage);

  std::vector<cv::Rect> faces;
  c.detectMultiScale(grayImage, faces, 1.1 ,1,CV_HAAR_SCALE_IMAGE, cv::Size(100,100));
  cv::Rect faceRect = faces[0];
  //draw rectangle around face
  //rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);
  cv::Mat faceROI = image(faceRect);
  cv::Point markerPos = locateMarker(faceROI);

  std::cout<< markerPos;
  std::cout<<faceRect;
  return 0;
}

int main1(int argc, const char** argv)
{
  if( argc != 2)
  {
    std::cout <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
    return -1;
  }
  config::load_config_file();
  Face f; Nose n; Eyes e; Mouth m;
  cv::Mat image, grayImage;
  image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  cvtColor(image, grayImage, CV_BGR2GRAY);
  equalizeHist(grayImage,grayImage);
  std::vector<cv::Rect> faces = f.detect(grayImage, cv::Size(100,100));
  std::cout<< faces.size();
  if(faces.size()==1){
    cv::Rect faceRect = faces[0];
    //draw rectangle around face
    rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);

    cv::Mat faceROI = image(faceRect);
    cv::Point markerPos = locateMarker(faceROI);
    cvtColor(faceROI, faceROI, CV_BGR2GRAY);

    imshow("test", faceROI);
    // detect nose
    //cv::Rect noseRect = n.detect(faceROI, cv::Size(40,40))[0];
    //extendRectangle(image, faceRect, noseRect);

    //detect mouth
    //cv::Rect mouthRect = m.detect(faceROI, cv::Size(40,40))[0];
    //extendRectangle(image, faceRect, mouthRect);

    //detect eyes
    cv::Rect eyeRect = e.detect(faceROI, cv::Size(30,30))[0];
    extendRectangle(image, faceRect, eyeRect);
  }
  else{
    std::cout<< "More than one faces found!";
    //log error
  }

  namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  imshow("Display window", image);
  cv::waitKey(0);
  return 0;
}

void extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect)
{
  cv::Point pt1(origRect.x + subRect.x, origRect.y + subRect.y);
  cv::Point pt2(pt1.x + subRect.width, pt1.y + subRect.height);
  rectangle(image, pt1, pt2, cv::Scalar(255,0,0),1,8,0);
}

cv::Point locateMarker(cv::Mat clrImg)
{
  cv::Mat imgHSV, bgIsolation;
  cv::Point mCoord;
  cvtColor(clrImg, imgHSV, cv::COLOR_BGR2HSV);
  int rh = 255, rl = 100, gh = 255, gl = 0, bh = 70, bl = 0;
  cv::inRange(imgHSV, cv::Scalar(bl, gl, rl), cv::Scalar(bh, gh, rh), bgIsolation);
  cv::Moments oMoments = cv::moments(bgIsolation);
  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;
  if(dArea > 0)
  {
    mCoord.x = dM10 / dArea;
    mCoord.y = dM01 / dArea;
  }
  return mCoord;
}
