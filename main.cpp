#include "includes/common.h"
#include "includes/FaceFeatures.h"
#include "includes/CSVLogger.h"
#include "includes/Utils.h"
#include <fstream>

void detect(cv::Mat img);
void process_frame(int frame, cv::Mat img);//todo add timestamp
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

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
int main(int argc, char** argv)
{
  config::load_config_file();//initialize config variables
  cv::Mat frame;
  if(cmdOptionExists(argv, argv+argc, "-i"))
  {
    char * filename = getCmdOption(argv, argv + argc, "-i");
    frame = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    process_frame(1, frame);
  }
  else if(cmdOptionExists(argv, argv+argc, "-v"))
  {
    char * filename = getCmdOption(argv, argv + argc, "-v");
    cv::VideoCapture videoSource;
    if(!videoSource.open(filename))
      exit(-1);
    videoSource.set(CV_CAP_PROP_CONVERT_RGB, 1);
    int frameCnt = videoSource.get(CV_CAP_PROP_FRAME_COUNT);
    for(int i=1; i <= frameCnt; i++)
    {
      videoSource >> frame;
      if(i>300 && !frame.empty())
      {
        process_frame(i,frame);
      }
      if(i==339)
      {
        imwrite("sample11.jpg", frame);
        break;
      }
    }
  }
  else
  {
    std::cerr << "Invalid option" << std::endl;
  }
  logger.flush();
  return 0;
}

void process_frame(int frame, cv::Mat img)
{
  logger.newRow();
  logger.addToRow("frame_no", Utils::toString(frame));
  detect(img);
}

void detect(cv::Mat image)
{
  Face f; Nose n; Eyes e; Mouth m;
  cv::Mat grayImage, equalized;
  cvtColor(image, grayImage, CV_BGR2GRAY);
  equalizeHist(grayImage,equalized);
  f.detect(equalized, cv::Size(100,100));
  if(f.detected()){
    cv::Rect nROI, eROI, mROI;
    cv::Rect faceRect = f.getRect();
    //draw rectangle around face
    logger.addToRow("face", faceRect);
    //rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);

    cv::Mat faceROI = image(faceRect);// need th color info locating marker
    cv::Point markerPos = Utils::locateMarker(faceROI);
    if(markerPos.x==0 && markerPos.y==0)
    {
      std::cerr<< "Marker not found!";
    }
    else
    {
      cv::Point markerPosGlobal = Utils::getGlobalMarkerPos(markerPos, faceRect);
      logger.addToRow(markerPosGlobal);
      //circle(image, markerPosGlobal, 3 , cv::Scalar( 252, 22, 120 ), -1, 8);//locate marker

      //detect mouth
      cv::Rect mouthROIrect = Utils::getROI(faceRect, 'm');
      m.detect(grayImage(mouthROIrect), cv::Size(40, 40));
      if(m.detected())
      {
        mROI = Utils::extendRectangle(mouthROIrect, m.getRect());
        //rectangle(image, mROI, cv::Scalar(255,0,0),1,8,0);
        logger.addToRow("mouth", mROI);
        if(mROI.contains(markerPosGlobal))
          logger.addToRow("marker_loc", "m");
      }

      //detect eyes
      cv::Rect eyeROIrect = Utils::getROI(faceRect, 'e');
      e.detect(grayImage(eyeROIrect), cv::Size(30,30));
      if(e.detected())
      {
        eROI = Utils::extendRectangle(eyeROIrect, e.getRect());
        //rectangle(image, eROI, cv::Scalar(255,0,0),1,8,0);
        logger.addToRow("eye", eROI);
        if(eROI.contains(markerPosGlobal))
          logger.addToRow("marker_loc", "e");
      }

      //detect nose
      cv::Rect noseROIrect = Utils::getROI(faceRect,'n');
      n.detect(grayImage(noseROIrect), cv::Size(40, 40));
      //note: probably need some exception handling if marker is found in more than one rects.
      //which probably will not happen :/
      if(n.detected())
      {
        nROI = Utils::extendRectangle(noseROIrect, n.getRect());
        if(e.detected())
          Utils::trimNRectE(nROI, eROI);
        if(m.detected())
          Utils::trimNRectM(nROI, mROI);
        //rectangle(image, nROI, cv::Scalar(255,0,0),1,8,0);
        logger.addToRow("nose", nROI);
        if(nROI.contains(markerPosGlobal))
          logger.addToRow("marker_loc", "n");
      }
    }
  }
  else
  {
    std::cerr<< "0 or more than one faces found...skipping frame!"<<std::endl;
  }

  //namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  //imshow("Display window", image);
  //cv::waitKey(0);
}
