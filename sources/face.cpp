#include "../includes/face.h"
#include "../includes/common.h"

Face::Face()
{
  face.load("/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_frontalface_alt2.xml");//put this in a config file
}

std::vector<cv::Rect> Face::detect(cv::Mat img)
{
  std::vector<cv::Rect> faces;
  face.detectMultiScale(img, faces, config::SCALE_FACTOR, 1, CV_HAAR_SCALE_IMAGE, cv::Size(200,200));
  return faces;
}

static void drawRectangle(cv::Mat img, cv::Mat detected_face)
{
 // rectangle(img, detected_face,Scalar(255,0,0),1,8,0);
}
