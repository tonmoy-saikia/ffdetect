#ifndef FACE_FEATURES_H
#define FACE_FEATURES_H

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
class Base
{
protected:
  cv::CascadeClassifier classifier;
  std::vector<cv::Rect> rects;
public:
  bool detected();
  void detect(cv::Mat im, cv::Size min_size);
  cv::Rect getRect();
};

class Face: public Base
{
public:
  Face();
};

class Nose: public Base
{
public:
  Nose();
};

class Eyes: public Base
{
public:
  Eyes();
};

class Mouth: public Base
{
public:
  Mouth();
};
#endif
