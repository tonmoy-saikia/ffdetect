#ifndef UTILS_H
#define UTILS_H
#include "../includes/ellipse.h"
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
typedef cv::Point_<float> pos_vector;
namespace Utils
{
  cv::Point locateMarker(cv::Mat clrImg);
  cv::Point getGlobalMarkerPos(cv::Point lpos, cv::Rect lrect);
  cv::Rect getROI(cv::Rect frect, char type);
  std::string toString(int);
  void printMarkerLocation(cv::Rect rect, cv::Point loc, std::string type);
  std::string getMarkerLocation(cv::Rect, cv::Rect, cv::Rect, cv::Point);
  cv::Point_<float> get_center_pt(std::vector< cv::Point_<float> > points);
  float getRegressionLineSlope(std::vector<cv::Point_<float> > points, cv::Point_<float>);
  void display_pts(cv::Mat &image, std::vector< cv::Point_<float> > points);
  pos_vector get_pv(cv::Point_<float> p1, cv::Point_<float> p2);
  void initialize_ellipse(pos_vector mj_scaling, pos_vector min_scaling, float slope, Ellipse &e, cv::Point_<float> center);
}
#endif
