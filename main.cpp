#include "includes/CSVLogger.h"
#include "includes/Utils.h"
#include "includes/LandmarkMapper.h"
#include <fstream>
#include <iostream>
#include <conio.h>
#include <vector>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/dir_nav.h>


void initialize_parser(dlib::command_line_parser &parser, int argc, char** argv);
void detect(cv::Mat img, std::string frame);
std::string get_bounding_ellipse(LandmarkMapper lm, cv::Point marker_loc, std::vector<EllipseROI> &elps);
EllipseROI get_face_ellipse(LandmarkMapper, float rotation);
EllipseROI interpolate_ellipse(cv::Mat &image, LandmarkMapper lm, std::string type);
void draw(cv::Mat &image, std::vector<EllipseROI> ellipses);
void process_video(std::string, dlib::command_line_parser &parser);


int main(int argc, char** argv)
{
  try
  {
    dlib::command_line_parser parser;
    initialize_parser(parser, argc, argv);
    if(parser.option("h"))
    {
      std::cout << "Usage: ffdetect [options] <video>" << std::endl;
      parser.print_options();
      return EXIT_SUCCESS;
    }
	if(parser.option("in_dir"))
	{
		 dlib::directory input_dir(get_option(parser, "in_dir", ""));
		 std::cout << "Processing files in directory: " << input_dir.name() << std::endl;
		 std::vector<dlib::file> files = input_dir.get_files();
		 for(int i=0; i<files.size(); i++)
			 process_video(files[i], parser);
	}
	else
	{	
		std::string filename = get_option(parser, "video", "");
		process_video(filename, parser);
	}
    
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


void process_video(std::string filename, dlib::command_line_parser &parser)
{
  std::string out_dir="./out";
  if(!filename.empty())
	out_dir = filename + "_out";
  std::cout << "Creating out dir..." << std::endl << out_dir << std::endl;
  dlib::create_directory(out_dir);
  CSVLogger logger(out_dir + "/logfile.csv");

  std::cout << "Processing... "<< filename << std::endl;
  bool webcam_mode = filename.empty();
  int refresh_interval = get_option(parser, "refresh_interval", 20);
  int start_frame, end_frame, framecnt;

  cv::VideoCapture cap;
  cap.set(CV_CAP_PROP_CONVERT_RGB, 1);

  if(webcam_mode)
  {
    cap.open(0);
  }
  else
  {
    cap.open(filename);
    framecnt = cap.get(CV_CAP_PROP_FRAME_COUNT);
    start_frame = get_option(parser, "start_frame", 0);
    end_frame = get_option(parser, "end_frame", framecnt);
    cap.set(CV_CAP_PROP_POS_FRAMES, start_frame);
    end_frame = end_frame>framecnt ? framecnt:end_frame;
  }

  cv::VideoWriter writer_object;
  if(parser.option("generate"))
  {
    int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int codec = CV_FOURCC('M', 'J', 'P', 'G');
    writer_object.open( out_dir + "/results.avi",
                        codec,
                        cap.get(CV_CAP_PROP_FPS),
                        cv::Size(frame_width,frame_height),
                        true);
  }

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
    if(!facefound || count%refresh_interval==0)
    {
      faces = detector(dimg);
      if(faces.size()>0)
      {
        facefound = true;
        tracker.start_track(dimg, centered_rect(faces[0], faces[0].width(), faces[0].height()));
        shape = pose_model(dimg, tracker.get_position());
      }
    }
    else
    {
      tracker.update(dimg);
      shape = pose_model(dimg, tracker.get_position());
    }
    cv::Point marker_loc = Utils::locateMarker(frame);

    dlib::rectangle r = tracker.get_position();
    cv::Rect frect = cv::Rect(cv::Point(r.left(),r.top()), cv::Point(r.right(),r.bottom()));
    //rectangle(frame, frect, cv::Scalar(255,0,0),1,8,0);

    //log extracted data
    std::vector<EllipseROI> ellipses;
    if(shape.num_parts()>0)
    {
      LandmarkMapper lm(shape);
      logger.addToRow(shape);
      logger.addToRow("marker_loc", get_bounding_ellipse(lm, marker_loc, ellipses));
      for(int i=0; i<ellipses.size(); i++)
      {
        logger.addToRow(ellipses[i]);
      }
    }
    if(!webcam_mode)
    {
      logger.addToRow("ts", Utils::toString(cap.get(CV_CAP_PROP_POS_MSEC)));
      logger.addToRow("marker_coord", marker_loc);
    }

    logger.addToRow("frame_no", Utils::toString(count));
    logger.addToRow("face_rect", tracker.get_position());

    if(parser.option("display"))
    {
      draw(frame, ellipses);
      imshow("disp window", frame);
    }
    if(writer_object.isOpened())
    {
      draw(frame, ellipses);
      writer_object << frame;
    }
	cv::waitKey(10);
    char keypress;
	if(kbhit())
	{
		std::cout<<"\n";
		std::cin.get(keypress);		
	}

    if(keypress == 'q')
    {
      std::cout << "Terminated!\n";
      break;
    }
    std::cout << "Processed " << count-start_frame << " out of "<<end_frame - start_frame << " frames\r";
    std::cout.flush();
  }
  std::cout << std::endl;
  cap.release();
  writer_object.release();
  logger.flush();//flush in batches, may overload the memory if there are large number of frame
}

void draw(cv::Mat &image, std::vector<EllipseROI> ellipses)
{
  for(int i=0; i<ellipses.size(); i++)
  {
    ellipses[i].draw(image);
  }
}

EllipseROI interpolate_ellipse(LandmarkMapper lm, std::string type)
{
  EllipseROI e(type);
  float slope = Utils::getRegressionLineSlope(lm.pmap[type], lm.cmap[type]);
  Utils::initialize_ellipse(lm.pvmap[type].maj, lm.pvmap[type].min, slope, e, lm.cmap[type]);
  return e;
}

EllipseROI get_face_ellipse(LandmarkMapper lm, float rotation)
{
  EllipseROI e("face");
  pos_vec major_axis_v = lm.pvmap["face"].maj;
  pos_vec minor_axis_v = lm.pvmap["face"].min;
  e.major_axis = sqrt(major_axis_v.x*major_axis_v.x + major_axis_v.y*major_axis_v.y);
  e.minor_axis = sqrt(minor_axis_v.x*minor_axis_v.x + minor_axis_v.y*minor_axis_v.y);
  e.rotation = rotation;
  e.center = lm.cmap["face"];
  return e;

}


std::string get_bounding_ellipse(LandmarkMapper lm, cv::Point marker_loc, std::vector<EllipseROI> &ellipses)
{
  EllipseROI m_elps  = interpolate_ellipse(lm, "mouth"); ellipses.push_back(m_elps);
  EllipseROI le_elps  = interpolate_ellipse(lm, "l_eye"); ellipses.push_back(le_elps);
  EllipseROI re_elps  = interpolate_ellipse(lm, "r_eye"); ellipses.push_back(re_elps);
  EllipseROI f_elps  = get_face_ellipse(lm, re_elps.rotation); ellipses.push_back(f_elps);

  std::string elps_name="";
  if(marker_loc != cv::Point(0,0))
  {
	  elps_name="p";
  }

  if(f_elps.encloses(marker_loc))
  {
    if(m_elps.encloses(marker_loc))
    {
      elps_name = "m";
    }else if(le_elps.encloses(marker_loc))
    {
      elps_name = "le";
    }else if(re_elps.encloses(marker_loc))
    {
      elps_name = "re";
    }
    else
    {
      elps_name = "f";
    }
  }
  return elps_name;
}

void initialize_parser(dlib::command_line_parser &parser, int argc, char** argv)
{
  parser.add_option("h", "Display this help message.");
  parser.add_option("video", "Input video file for processing (skipping this option will start webcam)", 1);
  parser.add_option("refresh_interval", "The number of frames to inerleave tracking and detecting (default 20)", 1);
  parser.add_option("start_frame", "start frame number number for video", 1);
  parser.add_option("end_frame", "end frame number for video", 1);
  //parser.add_option("image", "Process a single image", 1);
  parser.add_option("display", "Display processed images");
  parser.add_option("generate", "Generate output video with region of interests");
  parser.add_option("in_dir", "Input directory containing video files",1);
 
  parser.parse(argc, argv);
  const char* one_time_opts[] = {"h", "video", "start_frame", "end_frame", "display", "generate", "refresh_interval", "in_dir"};
  parser.check_one_time_options(one_time_opts); // Can't give an option more than once
  const char* incompatible[] = {"video", "in_dir"};
  parser.check_incompatible_options(incompatible);
}
