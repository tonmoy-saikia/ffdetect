#include "common.h"

class Base
{
protected:
  cv::CascadeClassifier classifier;
public:
  std::vector<cv::Rect> detect(cv::Mat im, cv::Size min_size);
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

