#include "../includes/Tracker.h"
/*
http://blog.cordiner.net/2011/05/03/object-tracking-using-a-kalman-filter-matlab/
http://opencvexamples.blogspot.com/2014/01/kalman-filter-implementation-tracking.html
*/
Tracker::Tracker():state(6,1),measurement(4,1)
{
  KF.init(6, 4, 0);//dim of state vector=6, dim of measurement vector 4
  state.setTo(cv::Scalar(0));
  measurement.setTo(cv::Scalar(0));
  KF.statePre = state;
  KF.transitionMatrix = *(cv::Mat_<float>(6, 6) << 1,0,0,0,1,0,
                                                   0,1,0,0,0,1,
                                                   0,0,1,0,1,0,
                                                   0,0,0,1,0,1,
                                                   0,0,0,0,1,0,
                                                   0,0,0,0,0,1
                                                   );//F
  KF.measurementMatrix = *(cv::Mat_<float>(4, 6) << 1,0,0,0,0,0,
                                                    0,1,0,0,0,0,
                                                    0,0,1,0,0,0,
                                                    0,0,0,1,0,0
                                                );//H

  KF.processNoiseCov = *(cv::Mat_<float>(6, 6) << 0.25,0,0,0,0.5,0,
                                                  0,0.25,0,0,0,0.5,
                                                  0,0,0.25,0,0.5,0,
                                                  0,0,0,0.25,0,0.5,
                                                  0.5,0,0.5,0,1,0,
                                                  0,0.5,0,0.5,0,1
                                                  )*1e-4;//Q .. is this right??

  setIdentity(KF.measurementNoiseCov, cv::Scalar::all(42.5));//R
  setIdentity(KF.errorCovPost, cv::Scalar::all(1e4));//P
}

void Tracker::update(cv::Rect rect, bool valid_measurement)
{
  KF.predict();
  if(valid_measurement)
  {
    measurement(0) = rect.x;
    measurement(1) = rect.y;
    measurement(2) = rect.x + rect.width;
    measurement(3) = rect.y + rect.height;
    KF.correct(measurement);
  }
}

cv::Rect Tracker::estimated_rect()
{
  cv::Mat estimated = KF.measurementMatrix * KF.statePost;
  cv::Rect est_rect =  cv::Rect(cv::Point(estimated.at<float>(0),estimated.at<float>(1)),
                       cv::Point(estimated.at<float>(2),estimated.at<float>(3))
                      );

}
