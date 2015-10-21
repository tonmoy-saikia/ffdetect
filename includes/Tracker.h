#ifndef TRACKER_H
#define TRACKER_H
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <iostream>

class Tracker
{
  public:
    cv::KalmanFilter KF;
    cv::Mat_<float> state;
    cv::Mat_<float> measurement;
  //public:
    Tracker();
    void update(cv::Rect, bool);
    cv::Rect estimated_rect();
};
#endif
