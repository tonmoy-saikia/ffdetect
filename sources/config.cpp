#include "../includes/config.h"


//http://stackoverflow.com/questions/10892167/proper-implementation-of-global-configuration


namespace config
{
  float SCALE_FACTOR = 1.3;
  std::string HAARCASCADE_FACE = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_frontalface_alt2.xml";
  std::string HAARCASCADE_MOUTH = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_mcs_mouth.xml";
  std::string HAARCASCADE_NOSE = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_mcs_nose.xml";
  //std::string HAARCASCADE_EYES = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_mcs_eyepair_big.xml";
  std::string HAARCASCADE_EYES = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
}

bool config::load_config_file()
{
      // Code to load and set the configuration variables
      SCALE_FACTOR = 2.0;
}
