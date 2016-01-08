#include "../includes/Utils.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdexcept>
#include <iostream>
#define PI 3.14159265

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
  //int rh = 255, rl = 200, gh = 0, gl = 0, bh = 0, bl = 0;
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

cv::Point_<float> Utils::get_center_pt(std::vector< cv::Point_<float> > points)
{
  float x,y;
  x=y=0.0;
  for(int i=0; i<points.size(); i++)
  {
    x+=points[i].x;
    y+=points[i].y;
  }
  x = x/points.size();
  y = y/points.size();
  return cv::Point_<float>(x,y);
}

float Utils::getRegressionLineSlope(std::vector<cv::Point_<float> > points, cv::Point_<float> center)
{

  float xy_sum=0.0; float xx_sum = 0.0;
  for(int i=0; i<points.size(); i++)
  {
    cv::Point_<float> pt = points[i];
    xy_sum = xy_sum + pt.x * pt.y;
    xx_sum = xx_sum + pt.x * pt.x;
  }

  float cxy = points.size()*center.x*center.y;
  float cxx = points.size()*center.x*center.x;
  float num = xy_sum - cxy;
  float den = xx_sum - cxx;
  return num/den;
}

void Utils::display_pts(cv::Mat &image, std::vector< cv::Point_<float> > points)
{
  for(int i=0; i<points.size(); i++)
  {
    circle( image, points[i], 1.0, cv::Scalar( 0, 0, 255 ), 2, 8 );
  }
}

pos_vector Utils::get_pv(cv::Point_<float> p1, cv::Point_<float> p2)
{
  return p2 - p1;
}

void Utils::initialize_ellipse(pos_vector mj_scaling, pos_vector min_scaling, float slope, Ellipse &e, cv::Point_<float> center)
{
  pos_vector major_axis_v, minor_axis_v;
  major_axis_v = pos_vector(sqrt(mj_scaling.x*mj_scaling.x + mj_scaling.y*mj_scaling.y),
      sqrt(mj_scaling.x*mj_scaling.x + mj_scaling.y*mj_scaling.y)*slope
      );

  if(major_axis_v.x == 0)
  {
    minor_axis_v = pos_vector(1.0, 0.0);
  }
  else
  {
    minor_axis_v = pos_vector(-major_axis_v.y/major_axis_v.x, 1.0);
  }

  minor_axis_v = minor_axis_v * sqrtf(min_scaling.x*min_scaling.x + min_scaling.y*min_scaling.y);
  e.major_axis = sqrt(major_axis_v.x*major_axis_v.x + major_axis_v.y*major_axis_v.y);
  e.minor_axis = sqrt(minor_axis_v.x*minor_axis_v.x + minor_axis_v.y*minor_axis_v.y);
  e.rotation = atan(slope) * 180 / PI;
  e.center = center;
}
