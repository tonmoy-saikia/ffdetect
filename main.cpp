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
#include <dlib/cmd_line_parser.h>

void initialize_parser(dlib::command_line_parser &parser, int argc, char** argv);
void detect(cv::Mat img, std::string frame);
std::string get_bounding_ellipse(cv::Mat &image, LandmarkMapper lm, cv::Point marker_loc);
Ellipse interpolate_ellipse(cv::Mat &image, LandmarkMapper lm, std::string type);
void process_video(std::string filename, int, int);
CSVLogger logger("logfile");

int main(int argc, char** argv)
{
  try
  {
    dlib::command_line_parser parser;
    initialize_parser(parser, argc, argv);
    std::cout << parser.number_of_arguments() << "\n";
    if(parser.option("h"))
    {
      std::cout << "Usage: ffdetect [options] <video|image file>" << std::endl;
      parser.print_options();
      return EXIT_SUCCESS;
    }


    std::string filename = get_option(parser, "video", "");//parser.option("video").argument();
    int start_frame = get_option(parser, "start_frame", 0);//parser.option("video").argument();
    int end_frame = get_option(parser, "end_frame", -1);//parser.option("video").argument();
    std::cout << filename;
    process_video(filename, start_frame, end_frame);

    logger.flush();//flush in batches, may overload the memory if there are large number of frames
  }
  catch(dlib::serialization_error& e)
  {
    std::cout << "You need dlib's default face landmarking model file to run this application." << std::endl;
    std::cout << "You can get it from the following URL: " << std::endl;
    std::cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << std::endl;
    std::cout << std::endl << e.what() << std::endl;
  }
  catch (std::exception& e)
  {
    std::cout << "\nexception thrown!" << std::endl;
    std::cout << e.what() << std::endl;
    std::cout << "\nTry the -h option for more information." << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

void process_video(std::string filename, int start_frame, int end_frame)
{
  cv::VideoCapture cap;
  cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
  bool webcam_mode = filename.length()==0;
  if(webcam_mode)
  {
    cap.open(0);
  }
  else
  {
    cap.open(filename);
    cap.set(CV_CAP_PROP_POS_FRAMES, start_frame);
    int framecnt = cap.get(CV_CAP_PROP_FRAME_COUNT);
    if(end_frame == -1)
    {
      end_frame = framecnt;
    }
    else
    {
      end_frame = end_frame>framecnt ? framecnt:end_frame;
    }
  }

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
  cv::Mat frame, gray_frame;

  int count = start_frame;
  std::cout << count;
  std::cout << "\n" << end_frame;
  while(webcam_mode || count < end_frame)
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


    //log extracted data

    if(shape.num_parts()>0)
    {
      LandmarkMapper lm(shape);
      std::string ml = get_bounding_ellipse(gray_frame, lm, marker_loc);
      logger.addToRow(shape);
      logger.addToRow("marker_loc", ml);
    }
    if(!webcam_mode)
    {
      logger.addToRow("ts", Utils::toString(cap.get(CV_CAP_PROP_POS_MSEC)));
    }

    logger.addToRow("frame_no", Utils::toString(count));
    logger.addToRow("face_rect", tracker.get_position());
    logger.addToRow(marker_loc);

    // display results
    imshow("disp window", frame);
    cv::waitKey(10);
  }
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

void initialize_parser(dlib::command_line_parser &parser, int argc, char** argv)
{
  parser.add_option("h", "Display this help message.");
  parser.add_option("video", "Input video file for processing", 1);
  parser.add_option("webcam", "Use webcam as video source");
  parser.add_option("start_frame", "start frame number number for video", 1);
  parser.add_option("end_frame", "end frame number for video", 1);
  parser.add_option("image", "Process a single image", 1);
  parser.add_option("display", "Display processed images");
  parser.add_option("generate", "Generate output video with region of interests",1);

  parser.parse(argc, argv);
  const char* one_time_opts[] = {"h", "video", "webcam", "start_frame", "end_frame", "image", "display", "generate"};
  parser.check_one_time_options(one_time_opts); // Can't give an option more than once
  const char* incompatible[] = {"video", "webcam", "image"};
  parser.check_incompatible_options(incompatible);
}
