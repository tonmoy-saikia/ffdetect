#include "includes/common.h"
#include "includes/FaceFeatures.h"
#include "includes/CSVLogger.h"
#include "includes/Utils.h"
#include <fstream>

void detect(cv::Mat img);

int main1(int argc, const char** argv)
{
  CSVLogger c("test1");
  c.newRow();
  c.addToRow("frame_no", "11");
  c.flush();
 /* config::load_config_file();*/
  //cv::CascadeClassifier c;
  ////c.load("/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_frontalface_alt2.xml");
  //c.load(config::HAARCASCADE_FACE);
  //cv::Mat image, grayImage;
  //cv::Point p;
  /*std::cout<< p;*/
  /* image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);*/
  //cvtColor(image, grayImage, CV_BGR2GRAY);
  //equalizeHist(grayImage, grayImage);

  //std::vector<cv::Rect> faces;
  //c.detectMultiScale(grayImage, faces, 1.1 ,1,CV_HAAR_SCALE_IMAGE, cv::Size(100,100));
  //cv::Rect faceRect = faces[0];
  ////draw rectangle around face
  //rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);
  //cv::Mat faceROI = image(faceRect);
  //cv::Point markerPos = locateMarker(faceROI);
  ////markerPos = locateMarker(faceROI);
  //circle(image, cv::Point(faceRect.x + markerPos.x, faceRect.y + markerPos.y), 3 , cv::Scalar( 252, 22, 120 ), -1, 8);
  ////circle(image, markerPos, 3 , cv::Scalar( 252, 22, 120 ), -1, 8);
  //namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  //cv::imshow("Display window", image);
  //cv::waitKey(0);
  /*std::cout<< faceRect.contains(cv::Point(faceRect.x + markerPos.x, faceRect.y + markerPos.y));*/
  return 0;
}

CSVLogger logger("test1");
int main(int argc, const char** argv)
{
  if( argc != 2)
  {
    std::cout <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
    return -1;
  }
  cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  config::load_config_file();//initialize config variables
  logger.newRow();
  logger.addToRow("frame_no", "11");
  detect(image);
  logger.flush();
  return 0;
}

void detect(cv::Mat image)
{
  Face f; Nose n; Eyes e; Mouth m;
  cv::Mat grayImage, equalized;
  cvtColor(image, grayImage, CV_BGR2GRAY);
  equalizeHist(grayImage,equalized);
  f.detect(equalized, cv::Size(100,100));
  if(f.detected()){
    cv::Rect faceRect = f.getRect();
    //draw rectangle around face
    rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);

    cv::Mat faceROI = image(faceRect);// need th color info locating marker
    cv::Point markerPos = Utils::locateMarker(faceROI);
    if(markerPos.x==0 && markerPos.y==0)
    {
      std::cerr<< "Marker not found!";
    }
    else
    {
      cv::Point markerPosGlobal = Utils::getGlobalMarkerPos(markerPos, faceRect);
      circle(image, markerPosGlobal, 3 , cv::Scalar( 252, 22, 120 ), -1, 8);
      //std::cout << markerPosGlobal;
      //cvtColor(faceROI, faceROI, CV_BGR2GRAY);

      // detect nose
      //getROI(grayImage, faceRect, 'n');
      cv::Rect noseROIrect = Utils::getROI(faceRect, 'n');
      n.detect(grayImage(noseROIrect), cv::Size(40,40));
      if(n.detected())
      {
        //extendRectangle(image, noseROIrect, n.getRect());
        Utils::printMarkerLocation(Utils::extendRectangle(image, noseROIrect, n.getRect()),markerPos,"nose");
      }

      //detect mouth
      cv::Rect mouthROIrect = Utils::getROI(faceRect, 'm');
      m.detect(grayImage(mouthROIrect), cv::Size(40,40));
      if(m.detected())
      {
        //extendRectangle(image, mouthROIrect, m.getRect());
        Utils::printMarkerLocation(Utils::extendRectangle(image, mouthROIrect, m.getRect()),markerPos,"mouth");
      }

      //detect eyes
      cv::Rect eyeROIrect = Utils::getROI(faceRect, 'e');
      e.detect(grayImage(eyeROIrect), cv::Size(30,30));
      if(e.detected())
      {
        //extendRectangle(image, eyeROIrect, e.getRect());
        //std::cout<< e.getRect() << std::endl;
        //std::cout<< markerPos;
        Utils::printMarkerLocation(Utils::extendRectangle(image, eyeROIrect, e.getRect()), markerPosGlobal,"eye");
      }
    }
  }
  else
  {
    std::cerr<< "0 or more than one faces found...skipping frame!"<<std::endl;
  }

  namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  imshow("Display window", image);
  cv::waitKey(0);
}
