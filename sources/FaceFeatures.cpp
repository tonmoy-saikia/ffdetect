#include "../includes/FaceFeatures.h"
#include "../includes/common.h"


std::vector<cv::Rect> Base::detect(cv::Mat img, cv::Size min_size)
{
  std::vector<cv::Rect> detectedObj;
  classifier.detectMultiScale(img, detectedObj, 1.1 , 1, CV_HAAR_SCALE_IMAGE, min_size);
  return detectedObj;
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
