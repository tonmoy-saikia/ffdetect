#include "includes/CSVLogger.h"
#include "includes/Utils.h"
#include "includes/LandmarkMapper.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

void detect(cv::Mat img, std::string frame);
char* getCmdOption(char ** begin, char ** end, const std::string & option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);
void process_video(char *filename);
std::string get_bounding_ellipse(cv::Mat &image, LandmarkMapper lm, cv::Point marker_loc);
Ellipse interpolate_ellipse(cv::Mat &image, LandmarkMapper lm, std::string type);

CSVLogger logger("logfile");

int main(int argc, char** argv)
{
  //Initialize config variables
  //config::load_config_file();
  //TODO improve cmd line parsing
  if(cmdOptionExists(argv, argv+argc, "-i"))
  {
    char * filename = getCmdOption(argv, argv + argc, "-i");
    //frame = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    //process_frame(1, frame);
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
    cv::VideoCapture cap;
    if(!cap.open(filename))
    {
      throw std::invalid_argument("Invalid filename");
    }
    cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
    //cap.set(CV_CAP_PROP_POS_FRAMES, 1500);//TODO change

    //int frame_width=   cap.get(CV_CAP_PROP_FRAME_WIDTH);
    //int frame_height=   cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    //cv::VideoWriter video("out.avi", static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)) , cap.get(CV_CAP_PROP_FPS), cv::Size(frame_width,frame_height),true);

    bool facefound = false;
    // Load face detection and pose estimation models.
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    std::cout << "Face Detector loaded." << std::endl;

    //Load shape model
    dlib::shape_predictor pose_model;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
    std::cout << "Face landmarks model loaded." << std::endl;


    dlib::correlation_tracker tracker;

    std::vector<dlib::rectangle> faces;
    dlib::full_object_detection shape;

    int count = 0;
    cv::Mat frame, gray_frame;

    while(count < 600)
    {
      // Grab a frame
      cap >> frame;
      count++;
      if(frame.empty())
      {
        std::cout << "Skipping empty frame. Frame no: " << count << std::endl;
        continue;
      }

      cvtColor(frame, gray_frame, CV_BGR2GRAY);
      //resize(gray_frame, gray_frame, cv::Size(640,480));

      //add a new row to the csv file
      logger.newRow();

      //don't modify gray_frame while using dimg.
      dlib::cv_image<uchar> dimg(gray_frame);
      //dlib::cv_image<dlib::bgr_pixel> cimg(temp);

      // Detect faces
      if(!facefound || count%20==0)
      {
        faces = detector(dimg);
        if(faces.size()>0)
        {
          facefound = true;
          tracker.start_track(dimg, centered_rect(faces[0], faces[0].width(), faces[0].height()));
          shape = pose_model(dimg, tracker.get_position());
          std::cout << "Refreshing..." << std::endl;
        }
      }
      else
      {
        tracker.update(dimg);
        shape = pose_model(dimg, tracker.get_position());
        std::cout << "Tracking..." << std::endl;
      }
      cv::Point marker_loc = Utils::locateMarker(frame);

      dlib::rectangle r = tracker.get_position();
      rectangle(frame, cv::Rect(cv::Point(r.left(),r.top()), cv::Point(r.right(),r.bottom())), cv::Scalar(255,0,0),1,8,0);
      if(shape.num_parts()>0)
      {
        LandmarkMapper lm(shape);
        std::string ml = get_bounding_ellipse(gray_frame, lm, marker_loc);
        logger.addToRow(shape);
        logger.addToRow("marker_loc", ml);
      }
      //log extracted data
      logger.addToRow("frame_no", Utils::toString(count));
      logger.addToRow("ts", Utils::toString(cap.get(CV_CAP_PROP_POS_MSEC)));
      logger.addToRow("face_rect", tracker.get_position());
      logger.addToRow(marker_loc);

      imshow("disp window", frame);
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

Ellipse interpolate_ellipse(cv::Mat &image, LandmarkMapper lm, std::string type)
{
  Ellipse e;
  Utils::display_pts(image, lm.pmap[type]);
  float slope = Utils::getRegressionLineSlope(lm.pmap[type], lm.cmap[type]);
  Utils::initialize_ellipse(lm.pvmap[type].maj, lm.pvmap[type].min, slope, e, lm.cmap[type]);
  e.draw(image);
  return e;
}

std::string get_bounding_ellipse(cv::Mat &image, LandmarkMapper lm, cv::Point marker_loc)
{
  Ellipse m_elps  = interpolate_ellipse(image, lm, "mouth");
  Ellipse le_elps  = interpolate_ellipse(image, lm, "l_eye");
  Ellipse re_elps  = interpolate_ellipse(image, lm, "r_eye");
  //Ellipse f_elps  = interpolate_ellipse(image, lm, "face");
  std::string elps_name="";

  if(m_elps.encloses(marker_loc))
  {
    elps_name = "m";
  }
  if(le_elps.encloses(marker_loc))
  {
    elps_name = "le";
  }
  if(re_elps.encloses(marker_loc))
  {
    elps_name = "re";
  }
  //if(f_elps.encloses(marker_loc))
  //{
   // elps_name = "f";
  //}
  return elps_name;
}
