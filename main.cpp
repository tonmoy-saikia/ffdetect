#include "includes/CSVLogger.h"
#include "includes/Utils.h"
#include "includes/config.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>

void detect(cv::Mat img, std::string frame);
void process_frame(int frame, cv::Mat img);//todo add timestamp
char* getCmdOption(char ** begin, char ** end, const std::string & option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);
void process_video(char *filename);

CSVLogger logger("logfile");

int main(int argc, char** argv)
{
  //Initialize config variables
  config::load_config_file();
  cv::Mat frame;
  //TODO improve cmd line parsing
  if(cmdOptionExists(argv, argv+argc, "-i"))
  {
    char * filename = getCmdOption(argv, argv + argc, "-i");
    frame = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    process_frame(1, frame);
  }
  else if(cmdOptionExists(argv, argv+argc, "-v"))
  {
    char * filename = getCmdOption(argv, argv + argc, "-v");
    process_video(filename);
  }
  else if(cmdOptionExists(argv, argv+argc, "-w"))
  {
    //process_video(filename, WEBCAM);//
  }
  else
  {
    std::cerr << "Invalid option" << std::endl;
  }
  logger.flush();
  return 0;
}

void process_video(char *filename)
{
  try
  {
    cv::VideoCapture cap;//(0)
    if(!cap.open(filename))
    {
      throw std::invalid_argument("Invalid filename");
    }
    cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
    cap.set(CV_CAP_PROP_POS_FRAMES, 1500);
    bool facefound = false;
    // Load face detection and pose estimation models.
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor pose_model;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

    dlib::correlation_tracker tracker;

    std::vector<dlib::rectangle> faces;
    dlib::full_object_detection shape;

    int count = 0;
    cv::Mat newframe;
    while(count < 600)
    {
      // Grab a frame
      cv::Mat temp;
      cap >> temp;
      if(temp.empty())
        continue;
      // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
      // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
      // long as temp is valid.  Also don't do anything to temp that would cause it
      // to reallocate the memory which stores the image as that will make cimg
      // contain dangling pointers.  This basically means you shouldn't modify temp
      // while using cimg.
      cv::Size size(640,480);//the dst image size,e.g.100x100
      resize(temp,temp,size);//resize image

      dlib::cv_image<dlib::bgr_pixel> cimg(temp);

      // Detect faces
      if(!facefound || count%20==0)
      {
        faces = detector(cimg);
        if(faces.size()>0)
        {
          tracker.start_track(cimg, centered_rect(faces[0],faces[0].width(),faces[0].height()));
          facefound = true;
          shape = pose_model(cimg, tracker.get_position());
        }
      }
      else
      {
        tracker.update(cimg);
        shape = pose_model(cimg, tracker.get_position());
      }
      count++;
      newframe = dlib::toMat(cimg);

      imshow("disp window", newframe);
      cv::waitKey(10);
    }
  }
  catch(dlib::serialization_error& e)
  {
    std::cout << "You need dlib's default face landmarking model file to run this application." << std::endl;
    std::cout << "You can get it from the following URL: " << std::endl;
    std::cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << std::endl;
    std::cout << std::endl << e.what() << std::endl;
  }
  catch(std::exception& e)
  {
    std::cout<< e.what() << std::endl;
  }
}
void process_frame(int frame, cv::Mat img)
{
  logger.newRow();
  logger.addToRow("frame_no", Utils::toString(frame));
  detect(img, Utils::toString(frame));
}

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

/*void detect(cv::Mat image, std::string frame_no)*/
//{

//Face f; Nose n; Eyes e; Mouth m;
//cv::Mat grayImage, equalized;
//cvtColor(image, grayImage, CV_BGR2GRAY);
//equalizeHist(grayImage,equalized);
//f.detect(equalized, cv::Size(100,100));
//if(f.detected()){

//cv::Rect nROI, eROI, mROI;
//cv::Rect faceRect = f.getRect();
////draw rectangle around face
//logger.addToRow("face", faceRect);
//rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);

//cv::Mat faceROI = image(faceRect);// need th color info locating marker
//cv::Point markerPos = Utils::locateMarker(faceROI);
//if(markerPos.x==0 && markerPos.y==0)
//{
//std::cerr<< "Marker not found!";
//}
//else
//{
//cv::Point markerPosGlobal = Utils::getGlobalMarkerPos(markerPos, faceRect);
//logger.addToRow(markerPosGlobal);
//circle(image, markerPosGlobal, 3 , cv::Scalar( 252, 22, 120 ), -1, 8);//locate marker

////detect mouth
//cv::Rect mouthROIrect = Utils::getROI(faceRect, 'm');
//m.detect(grayImage(mouthROIrect), cv::Size(40, 40));
//if(m.detected())
//{
//mROI = Utils::extendRectangle(mouthROIrect, m.getRect());
//rectangle(image, mROI, cv::Scalar(255,0,0),1,8,0);
//logger.addToRow("mouth", mROI);
//if(mROI.contains(markerPosGlobal))
//logger.addToRow("marker_loc", "m");
//}

////detect eyes
//cv::Rect eyeROIrect = Utils::getROI(faceRect, 'e');
//e.detect(grayImage(eyeROIrect), cv::Size(30,30));
//if(e.detected())
//{
//eROI = Utils::extendRectangle(eyeROIrect, e.getRect());
//rectangle(image, eROI, cv::Scalar(255,0,0),1,8,0);
//logger.addToRow("eye", eROI);
//if(eROI.contains(markerPosGlobal))
//logger.addToRow("marker_loc", "e");
//}

////detect nose
//cv::Rect noseROIrect = Utils::getROI(faceRect,'n');
//n.detect(grayImage(noseROIrect), cv::Size(40, 40));
////note: probably need some exception handling if marker is found in more than one rects.
////which probably will not happen :/
//if(n.detected())
//{
//nROI = Utils::extendRectangle(noseROIrect, n.getRect());
//if(e.detected())
//Utils::trimNRectE(nROI, eROI);
//if(m.detected())
//Utils::trimNRectM(nROI, mROI);
//rectangle(image, nROI, cv::Scalar(255,0,0),1,8,0);
//logger.addToRow("nose", nROI);
//if(nROI.contains(markerPosGlobal))
//logger.addToRow("marker_loc", "n");
//}
//}
//std::string filename = "./found/"+frame_no + "found.jpg";
//imwrite(filename, image);
//}
//else
//{
//std::string filename = "./nfound/"+frame_no + "notfound.jpg";
//imwrite(filename,image);
//std::cerr<< "0 or more than one faces found...skipping frame!"<<std::endl;
//}

////namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
////imshow("Display window", image);
////cv::waitKey(0);
/*}*/
