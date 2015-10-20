#ifndef UTILS_H
#define UTILS_H
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
namespace Utils
{
  cv::Rect extendRectangle(cv::Rect origRect, cv::Rect subRect);
  cv::Point locateMarker(cv::Mat clrImg);
  cv::Point getGlobalMarkerPos(cv::Point lpos, cv::Rect lrect);
  cv::Rect getROI(cv::Rect frect, char type);
  std::string toString(int);
  void printMarkerLocation(cv::Rect rect, cv::Point loc, std::string type);
  std::string getMarkerLocation(cv::Rect, cv::Rect, cv::Rect, cv::Point);
  void trimNRectM(cv::Rect&, cv::Rect);
  void trimNRectE(cv::Rect&, cv::Rect);
}
#endif
