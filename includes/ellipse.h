#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <string.h>
#include <opencv2/core/core.hpp>

class EllipseROI{
  public:
    std::string type;
    EllipseROI(std::string t);
    float major_axis, minor_axis, rotation;
    cv::Point_<float> center;
    void draw(cv::Mat &);
    bool encloses(cv::Point);
};
#endif
