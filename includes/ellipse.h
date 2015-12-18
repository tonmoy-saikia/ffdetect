#ifndef ELLIPSE
#define ELLIPSE
#include <opencv2/core/core.hpp>
class Ellipse{
  public:
    float major_axis, minor_axis, rotation;
    cv::Point_<float> center;
    Ellipse();
    void draw(cv::Mat &);
    bool encloses(cv::Point);
};
#endif
