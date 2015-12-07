#include "includes/common.h"
#include "includes/FaceFeatures.h"
#include "includes/Utils.h"
#include <fstream>

cv::Rect matchTemplate1(cv::Mat image, cv::Mat tmp);

int main(int argc, char** argv)
{
  cv::VideoCapture videoSource;
  if(!videoSource.open(argv[1]))
    exit(-1);
  config::load_config_file();
  videoSource.set(CV_CAP_PROP_CONVERT_RGB, 1);
  videoSource.set(CV_CAP_PROP_POS_FRAMES, 500);
  cv::Mat frame, fTemplate;
  bool hasTemplate = false;
  Face f;
  std::vector<cv::Mat> trainImages;
  for(;;)
  {
    videoSource >> frame;
    cv::Rect output;
    if(!frame.empty()){
      /*iif(hasTemplate)*/
      //{
        //output = matchTemplate(frame, fTemplate);
      //}
      //else
      //{
        f.detect(frame, cv::Size(100,100));
        if(f.detected()){
          cv::Rect faceRect = f.getRect();
          fTemplate = frame(faceRect);
          hasTemplate = true;
          imwrite("sample11.jpg", fTemplate);
          exit(0);
          //rectangle(frame, faceRect, cv::Scalar(255,0,0),1,8,0);
        }
        if(hasTemplate)
        {
          output = matchTemplate1(frame, fTemplate);
        }
      /*}*/

      //rectangle(frame, output, cv::Scalar(0,255,0),1,8,0);
      namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
      //imshow("Display window", fTemplate);
      cv::waitKey(1);
    }
  }
  return 0;
}

cv::Rect matchTemplate1(cv::Mat img, cv::Mat templ)
{
  cvtColor(img, img, CV_BGR2GRAY);
  cvtColor(templ, templ, CV_BGR2GRAY);
  cv::Mat result;
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;
  result.create( result_rows, result_cols, CV_32FC1 );
  matchTemplate( img, templ, result, CV_TM_CCORR_NORMED);

  imshow("Display window", result);
  normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
  double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
  cv::Point matchLoc;
  matchLoc = maxLoc;
  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
  return cv::Rect(matchLoc, cv::Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows));
}
