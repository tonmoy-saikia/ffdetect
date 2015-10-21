#include "opencv2/imgproc/imgproc.hpp"
#include "includes/config.h"
#include "opencv2/highgui/highgui.hpp"
#include "includes/FaceFeatures.h"
#include "includes/Tracker.h"
#include "includes/Utils.h"
void update_tracker(Tracker &t, cv::Rect rect)
{
  t.measurement(0) = rect.x;
  t.measurement(1) = rect.y;
  t.measurement(2) = rect.x + rect.width;
  t.measurement(3) = rect.y + rect.height;
  t.KF.correct(t.measurement);

}

void draw_estimated(Tracker t, cv::Mat frame)
{
  cv::Mat estimated = t.KF.measurementMatrix * t.KF.statePost;
  cv::Rect est_rect = cv::Rect(cv::Point(estimated.at<float>(0),estimated.at<float>(1)),
                      cv::Point(estimated.at<float>(2),estimated.at<float>(3))
                      );

  rectangle(frame, est_rect, cv::Scalar(0,255,0),1,8,0);
}

int main(int argc, const char** argv)
{
  cv::VideoCapture videoSource;
  if(!videoSource.open(argv[1])){
    exit(-1);
  }
  videoSource.set(CV_CAP_PROP_CONVERT_RGB, 1);
  videoSource.set(CV_CAP_PROP_POS_FRAMES, 500);
  cv::Mat frame;
  config::load_config_file();//initialize config variables
  Face f; Eyes e; Nose n; Mouth m;
  Tracker t1, t2, t3, t4;
  cv::Mat estimated;
  for(;;)
  {
    videoSource >> frame;
    if(!frame.empty()){
      f.detect(frame, cv::Size(100,100));
      t1.KF.predict(); t2.KF.predict(); t3.KF.predict(); t4.KF.predict();
      //t.update(f.getRect(), f.detected());
      if(f.detected())
      {
        update_tracker(t1, f.getRect());
        rectangle(frame, f.getRect(), cv::Scalar(255,0,0),1,8,0);
        cv::Rect eyeROIrect = Utils::getROI(f.getRect(), 'e');
        e.detect(frame(eyeROIrect), cv::Size(30,30));
        if(e.detected())
        {
          cv::Rect eROI = Utils::extendRectangle(eyeROIrect, e.getRect());
          rectangle(frame, eROI, cv::Scalar(255,0,0),1,8,0);
          update_tracker(t2, eROI);
        }
       cv::Rect noseROIrect = Utils::getROI(f.getRect(), 'n');
       n.detect(frame(noseROIrect), cv::Size(40, 40));
       if(n.detected())
       {
          cv::Rect nROI = Utils::extendRectangle(noseROIrect, n.getRect());
          rectangle(frame, nROI, cv::Scalar(255,0,0),1,8,0);
          update_tracker(t3, nROI);
       }
       cv::Rect mouthROIrect = Utils::getROI(f.getRect(), 'm');
       m.detect(frame(mouthROIrect), cv::Size(40, 40));
       if(m.detected())
       {
          cv::Rect mROI = Utils::extendRectangle(mouthROIrect, m.getRect());
          rectangle(frame, mROI, cv::Scalar(255,0,0),1,8,0);
          update_tracker(t4, mROI);
       }

      }
      draw_estimated(t1,frame);
      draw_estimated(t2,frame);
      draw_estimated(t3,frame);
      draw_estimated(t4,frame);
      //estimated = t1.KF.measurementMatrix * t1.KF.statePost;
      //cv::Rect est_rect = cv::Rect(cv::Point(estimated.at<float>(0),estimated.at<float>(1)),
     //     cv::Point(estimated.at<float>(2),estimated.at<float>(3))
      //    );

     // rectangle(frame, est_rect, cv::Scalar(0,255,0),1,8,0);
      namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
      imshow("Display window", frame);
      cv::waitKey(1);
    }
  }
  return 0;
}
