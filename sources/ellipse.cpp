#include "../includes/ellipse.h"
Ellipse::Ellipse(){}
bool Ellipse::encloses(cv::Point pt)
{
  return true;
}

void Ellipse::draw(cv::Mat &image)
{
  ellipse( image, center, cv::Size(major_axis, minor_axis), rotation ,0,360, cv::Scalar( 255, 0, 0 ), 1, 8 );
}
