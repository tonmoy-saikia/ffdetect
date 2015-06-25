#include "../includes/config.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
//http://stackoverflow.com/questions/10892167/proper-implementation-of-global-configuration


namespace config
{
  float SCALE_FACTOR = 1.1;
  std::string HAARCASCADE_FACE = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_frontalface_alt2.xml";
  std::string HAARCASCADE_MOUTH = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_mcs_mouth.xml";
  std::string HAARCASCADE_NOSE = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_mcs_nose.xml";
  std::string HAARCASCADE_EYES = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_mcs_eyepair_big.xml";
  //std::string HAARCASCADE_EYES = "/home/tonmoy/opencv/opencv-2.4.10/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
}

bool config::load_config_file()
{
  std::ifstream in_stream;
  std::string str;
  in_stream.open("/home/tonmoy/my_repo/ffdetect/config/init");
  while(!in_stream.eof())
  {
    while(std::getline(in_stream, str))
    {
      std::string::size_type begin = str.find_first_not_of(" \f\t\v");
      if(begin == std::string::npos)
        continue;
      //Skips #
      if(std::string("#").find(str[begin]) != std::string::npos)
        continue;
      std::string key, value;
      str = trim_string(str);
      std::size_t pos = str.find("=");
      key = str.substr(0, pos);
      std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      value = trim_string(str.substr(pos+1));
      load_variable(key, value);
    }
  }
  in_stream.close();
}


void config::load_variable(std::string key, std::string value)
{
  if(key == "SCALE_FACTOR")
    SCALE_FACTOR = ::atof(value.c_str());
}

std::string trim_string(std::string str)
{
  std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
  str.erase(end_pos, str.end());
  return str;
}
