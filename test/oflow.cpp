#include "includes/common.h"
#include "includes/FaceFeatures.h"
#include "includes/Utils.h"
#include "opencv2/video/tracking.hpp"
#include <fstream>
#define MAX_COUNT 50
void drawPoints(cv::Mat &image, std::vector<cv::Point2f>);

void refreshPoints(std::vector<cv::Point2f> &points1, std::vector<cv::Point2f> &points0,std::vector<uchar> matchStatus);
int main(int argc, char** argv)
{
  cv::Mat prevgray, gray, frame, mask;
  cv::VideoCapture videoSource;
  cv::Rect prevrect, currect, prev_eye_rect, current_eye_rect;
  if(!videoSource.open(argv[1]))
    exit(-1);
  videoSource.set(CV_CAP_PROP_CONVERT_RGB, 1);
  videoSource.set(CV_CAP_PROP_POS_FRAMES, 1300);
  config::load_config_file();
  Face f; Eyes e;
  std::vector<cv::Point2f> points[2];
  cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
  cv::Size subPixWinSize(10,10), winSize(31,31);
  bool initialized = false;
  int count=0;
  for(;;)
  {
    count++;
    videoSource >> frame;
    //detect face
    if(frame.empty())
      continue;
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    f.detect(gray, cv::Size(100,100));
    e.detect(gray, cv::Size(30, 30));
    if(f.detected() && count%5==0)
    {
      points[0].clear();
      points[1].clear();
      //create mask
      cv::Mat mask(gray.size(), CV_8UC1, cv::Scalar::all(0));
      mask(f.getRect()).setTo(cv::Scalar::all(255));
      //get good features in masked ROI
      cv::goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, mask, 3, 0, 0.04);
      cv::cornerSubPix(gray, points[1], subPixWinSize, cv::Size(-1,-1), termcrit);
      initialized = true;
      prevrect = f.getRect();
      currect = prevrect;
      if(e.detected())
      {
        prev_eye_rect = e.getRect();
        current_eye_rect = prev_eye_rect;
        rectangle(frame, e.getRect(), cv::Scalar(255,255,0),1,8,0);
      }
    }
    if(!points[0].empty())
    {
      std::vector<uchar> status;
      std::vector<float> err;
      if(prevgray.empty())
        gray.copyTo(prevgray);
      calcOpticalFlowPyrLK(prevgray, gray, points[0], points[1], status, err, winSize,
          3, termcrit, 0, 0.001);
      refreshPoints(points[1], points[0], status);
      drawPoints(frame, points[1]);
      //points1 and points 0 should always be equal
      //and should have ateast three points
      cv::Mat tx;
      if(points[0].size()>3)
      {
        tx = cv::estimateRigidTransform(points[0],points[1],false);
        cv::Point coord;
        if(!tx.empty())
        {
          //coord.x = prevrect.x*tx.at<double>(0,0) + prevrect.y*tx.at<double>(0,1)+ tx.at<double>(0,2);
          //coord.y =  prevrect.x*tx.at<double>(1,0) + prevrect.y*tx.at<double>(1,1)+ tx.at<double>(1,2);
          //currect = cv::Rect(coord.x,coord.y, prevrect.width, prevrect.height);
          //prevrect = currect;
          coord.x = prev_eye_rect.x*tx.at<double>(0,0) + prev_eye_rect.y*tx.at<double>(0,1)+ tx.at<double>(0,2);
          coord.y =  prev_eye_rect.x*tx.at<double>(1,0) + prev_eye_rect.y*tx.at<double>(1,1)+ tx.at<double>(1,2);
          current_eye_rect = cv::Rect(coord.x,coord.y, prev_eye_rect.width, prev_eye_rect.height);
          prev_eye_rect = current_eye_rect;
         }
      }
    }
    std::swap(points[1], points[0]);
    cv::swap(prevgray, gray);
    cv::putText(frame, "sds", cv::Point(10,10),cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0,255,0));
    //rectangle(frame, currect, cv::Scalar(0,255,0),1,8,0);
    rectangle(frame, current_eye_rect, cv::Scalar(0,255,0),1,8,0);
    imshow("Display window", frame);
    cv::waitKey(30);
  }
  return 0;
}

void drawPoints(cv::Mat &image, std::vector<cv::Point2f> points)
{
  for(int i=0; i<points.size();i++)
  {
    circle( image, points[i], 3, cv::Scalar(0,255,0), -1, 8);
  }
}

void refreshPoints(std::vector<cv::Point2f> &points1, std::vector<cv::Point2f> &points0,std::vector<uchar> matchStatus)
{
  size_t i,k;
  for(i=0,k=0; i<points1.size(); i++)
  {
    if(!matchStatus[i])
      continue;
    points1[k] = points1[i];
    points0[k] = points0[i];
    k++;
  }
  points0.resize(k);
  points1.resize(k);
}

int main_dense_of_example(int argc, char** argv)
{
  cv::Mat prevgray, gray, flow, cflow, frame;
  cv::Mat newFrame;
  cv::VideoCapture videoSource;
  if(!videoSource.open(0))
    exit(-1);
  config::load_config_file();
  //videoSource.set(CV_CAP_PROP_CONVERT_RGB, 1);
  //videoSource.set(CV_CAP_PROP_POS_FRAMES, 500);
  Face f;
  bool track = false;
  cv::Rect r;
  for(;;)
  {
    videoSource >> frame;
    if(!frame.empty()){
      cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
      if(!track){
        f.detect(gray, cv::Size(100,100));
        if(f.detected()){
         r = f.getRect();
          track = true;
         }
      }

      if(track && prevgray.data)
      {
        calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        //cv::Mat map(flow.size(), CV_32FC2);
        cv::Point2f fl = flow.at<cv::Point2f>(r.x, r.y);
        cv::Point2f newcoord =cv::Point(r.x + fl.x, r.y+fl.y);
        cv::Rect newrect = cv::Rect(newcoord, cv::Point(newcoord.x + r.width, newcoord.y + r.height));
        r.x = newcoord.x;
        r.y = newcoord.y;
        rectangle(frame, newrect, cv::Scalar(0,255,0),1,8,0);
        //map.at<cv::Point2f>(r.y, r.x) = cv::Point2f(r.x + f.x, r.y + f.y);
        //cv::remap(prevgray, newFrame, map, cv::Mat(), CV_INTER_LINEAR);
        //for (int y = 0; y < map.rows; ++y)
        //{
          //for (int x = 0; x < map.cols; ++x)
          //{
            //cv::Point2f f = flow.at<cv::Point2f>(y, x);
            //map.at<cv::Point2f>(y, x) = cv::Point2f(x + f.x, y + f.y);
          //}
        //}
        //cv::remap(prevgray, newFrame, map, cv::Mat(), CV_INTER_LINEAR);
        //namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
        imshow("Display window", frame);
      }
      //rectangle(frame, output, cv::Scalar(0,255,0),1,8,0);
      std::swap(prevgray, gray);
      cv::waitKey(1);
    }
  }
  return 0;
}


