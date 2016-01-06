#include "../includes/ellipse.h"
Ellipse::Ellipse(){}
bool Ellipse::encloses(cv::Point pt)
{
  float eval = pow(((pt.x - center.x)*cos(rotation) + (pt.y - center.y)*sin(rotation))/major_axis,2) +
               pow(((pt.x - center.x)*sin(rotation) - (pt.y - center.y)*cos(rotation))/minor_axis,2);

  return eval<=1.0;
}

void Ellipse::draw(cv::Mat &image)
{
  ellipse( image, center, cv::Size(major_axis, minor_axis), rotation ,0,360, cv::Scalar( 255, 0, 0 ), 1, 8 );
}
