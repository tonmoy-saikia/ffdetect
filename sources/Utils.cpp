#include "../includes/Utils.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdexcept>
#include <iostream>
cv::Rect Utils::extendRectangle(cv::Rect origRect, cv::Rect subRect)
{
  cv::Point pt1(origRect.x + subRect.x, origRect.y + subRect.y);
  cv::Point pt2(pt1.x + subRect.width, pt1.y + subRect.height);
  return cv::Rect(pt1,pt2);
}

std::string Utils::toString(int num)
{
  std::stringstream ss;
  ss << num;
  return ss.str();
}
cv::Point Utils::locateMarker(cv::Mat clrImg)
{
  cv::Mat bgIsolation;
  cv::Point mCoord;
  //cvtColor(clrImg, imgHSV, cv::COLOR_BGR2HSV);//no hsv for now
  int rh = 255, rl = 100, gh = 255, gl = 0, bh = 70, bl = 0;
  cv::inRange(clrImg, cv::Scalar(bl, gl, rl), cv::Scalar(bh, gh, rh), bgIsolation);
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

cv::Point Utils::getGlobalMarkerPos(cv::Point lpos, cv::Rect r)
{
  return cv::Point(lpos.x + r.x, lpos.y + r.y);
}

cv::Rect Utils::getROI(cv::Rect frect, char type)
{
  cv::Rect rect;
  if(type == 'm')
  {
    rect = cv::Rect(frect.x,
        (int)(frect.y + frect.height*0.6666),
        frect.width,
        (int)(frect.height/3));
  }
  else if(type == 'n')
  {

    rect = cv::Rect(frect.x,
        (int)(frect.y + frect.height*0.3),
        frect.width,
        (int)(frect.height*0.666));
  }
  else if(type == 'e')
  {
    rect = cv::Rect(frect.x,
        (int)(frect.y + frect.height*0.1),
        frect.width,
        (int)(frect.height/2));
  }
  else
  {
    throw std::invalid_argument("Invalid type for roi. Valid types are m,n,e");
  }
  return rect;
}

void Utils::printMarkerLocation(cv::Rect rect, cv::Point loc, std::string type)
{
  if(rect.contains(loc))
  {
    std::cout<< "Marker in "<<type << "\n";
  }
}

void Utils::trimNRectE(cv::Rect &nROI, cv::Rect eROI)
{
  int offset = 1;
  nROI.y = eROI.y + eROI.height + offset;
}

void Utils::trimNRectM(cv::Rect &nROI, cv::Rect mROI)
{
  int offset = 1;
  nROI.height = mROI.y - nROI.y - offset;
}
