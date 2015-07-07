#include "../includes/FaceFeatures.h"
#include "../includes/common.h"


void Base::detect(cv::Mat img, cv::Size min_size)
{
  classifier.detectMultiScale(img, rects, config::SCALE_FACTOR, 1,CV_HAAR_SCALE_IMAGE, min_size);
}

bool Base::detected()
{
  return rects.size()>0;
}

cv::Rect Base::getRect()
{
  return rects[0];
}

Face::Face()
{
  classifier.load(config::HAARCASCADE_FACE);
}

Nose::Nose()
{
  classifier.load(config::HAARCASCADE_NOSE);
}

Mouth::Mouth()
{
  classifier.load(config::HAARCASCADE_MOUTH);
}

Eyes::Eyes()
{
  classifier.load(config::HAARCASCADE_EYES);
}
