#include "includes/common.h"
#include "includes/FaceFeatures.h"
#include <stdexcept>
#include <fstream>

cv::Rect extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect);
cv::Point locateMarker(cv::Mat clrImg);
void detect(cv::Mat img);
cv::Point getGlobalMarkerPos(cv::Point lpos, cv::Rect lrect);
cv::Rect getROI(cv::Rect frect, char type);
void printMarkerLocation(cv::Rect, cv::Point, std::string);
//cv::Mat getEyesROI(cv::Rect frect, cv::Mat face);
//cv::Mat getNoseROI(cv::Mat face);

int main1(int argc, const char** argv)
{
  config::load_config_file();
  cv::CascadeClassifier c;
  //c.load("/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_frontalface_alt2.xml");
  c.load(config::HAARCASCADE_FACE);
  cv::Mat image, grayImage;
  cv::Point p;
  std::cout<< p;
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

int main(int argc, const char** argv)
{
  if( argc != 2)
  {
    std::cout <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
    return -1;
  }
  cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  config::load_config_file();//initialize config variables
  detect(image);
  return 0;
}

cv::Rect extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect)
{
  cv::Point pt1(origRect.x + subRect.x, origRect.y + subRect.y);
  cv::Point pt2(pt1.x + subRect.width, pt1.y + subRect.height);
  rectangle(image, pt1, pt2, cv::Scalar(255,0,0),1,8,0);
  return cv::Rect(pt1,pt2);
}

cv::Point locateMarker(cv::Mat clrImg)
{
  cv::Mat bgIsolation;
  cv::Point mCoord;
  //cvtColor(clrImg, imgHSV, cv::COLOR_BGR2HSV);//no hsv for now
  int rh = 255, rl = 100, gh = 255, gl = 0, bh = 70, bl = 0;
  cv::inRange(clrImg, cv::Scalar(bl, gl, rl), cv::Scalar(bh, gh, rh), bgIsolation);
  cv::Moments oMoments = cv::moments(bgIsolation);
  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;
  if(dArea > 0)
  {
    mCoord.x = dM10 / dArea;
    mCoord.y = dM01 / dArea;
  }
  return mCoord;
}

cv::Point getGlobalMarkerPos(cv::Point lpos, cv::Rect r)
{
  return cv::Point(lpos.x + r.x, lpos.y + r.y);
}

cv::Rect getROI(cv::Rect frect, char type)
{
  cv::Rect rect;
  if(type == 'm')
  {
    rect = cv::Rect(frect.x,
        (int)(frect.y + frect.height*0.6666),
        frect.width,
        (int)(frect.height/3));
  }
  else if(type == 'n')
  {

    rect = cv::Rect(frect.x,
        (int)(frect.y + frect.height*0.3),
        frect.width,
        (int)(frect.height*0.666));
  }
  else if(type == 'e')
  {
    rect = cv::Rect(frect.x,
        (int)(frect.y + frect.height*0.1),
        frect.width,
        (int)(frect.height/2));
  }
  else
  {
    throw std::invalid_argument("Invalid type for roi. Valid types are m,n,e");
  }
  return rect;
}

void printMarkerLocation(cv::Rect rect, cv::Point loc, std::string type)
{
  if(rect.contains(loc))
  {
    std::cout<< "Marker in "<<type << "\n";
  }
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
    cv::Point markerPos = locateMarker(faceROI);
    if(markerPos.x==0 && markerPos.y==0)
    {
      std::cerr<< "Marker not found!";
    }
    else
    {
      cv::Point markerPosGlobal = getGlobalMarkerPos(markerPos, faceRect);
      circle(image, markerPosGlobal, 3 , cv::Scalar( 252, 22, 120 ), -1, 8);
      //std::cout << markerPosGlobal;
      //cvtColor(faceROI, faceROI, CV_BGR2GRAY);

      // detect nose
      //getROI(grayImage, faceRect, 'n');
      cv::Rect noseROIrect = getROI(faceRect, 'n');
      n.detect(grayImage(noseROIrect), cv::Size(40,40));
      if(n.detected())
      {
        //extendRectangle(image, noseROIrect, n.getRect());
        printMarkerLocation(extendRectangle(image, noseROIrect, n.getRect()),markerPos,"nose");
      }

      //detect mouth
      cv::Rect mouthROIrect = getROI(faceRect, 'm');
      m.detect(grayImage(mouthROIrect), cv::Size(40,40));
      if(m.detected())
      {
        //extendRectangle(image, mouthROIrect, m.getRect());
        printMarkerLocation(extendRectangle(image, mouthROIrect, m.getRect()),markerPos,"mouth");
      }

      //detect eyes
      cv::Rect eyeROIrect = getROI(faceRect, 'e');
      e.detect(grayImage(eyeROIrect), cv::Size(30,30));
      if(e.detected())
      {
        //extendRectangle(image, eyeROIrect, e.getRect());
        //std::cout<< e.getRect() << std::endl;
        //std::cout<< markerPos;
        printMarkerLocation(extendRectangle(image, eyeROIrect, e.getRect()), markerPosGlobal,"eye");
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
