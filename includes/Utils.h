#ifndef UTILS_H
#define UTILS_H
#include <string>
#include "common.h"

namespace Utils
{
  cv::Rect extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect);
  cv::Point locateMarker(cv::Mat clrImg);
  cv::Point getGlobalMarkerPos(cv::Point lpos, cv::Rect lrect);
  cv::Rect getROI(cv::Rect frect, char type);
  std::string toString(int);
  void printMarkerLocation(cv::Rect rect, cv::Point loc, std::string type);
}
#endif
