#include "../includes/config.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
//http://stackoverflow.com/questions/10892167/proper-implementation-of-global-configuration


namespace config
{
  //Default config values go here
  float SCALE_FACTOR = 1.1;
  std::string HAARCASCADE_EYES;
  std::string HAARCASCADE_MOUTH;
  std::string HAARCASCADE_NOSE;
  std::string HAARCASCADE_FACE;
}

bool config::load_config_file()
{
  std::ifstream in_stream;
  std::string str;
  in_stream.open("/home/tonmoy/my_repo/ffdetect/config/init");//config file loc
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
  if(key == "HAARCASCADE_FACE")
    HAARCASCADE_FACE = value;
  if(key == "HAARCASCADE_MOUTH")
    HAARCASCADE_MOUTH = value;
  if(key == "HAARCASCADE_NOSE")
    HAARCASCADE_NOSE = value;
  if(key == "HAARCASCADE_EYES")
    HAARCASCADE_EYES = value;
}

std::string trim_string(std::string str)
{
  std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
  str.erase(end_pos, str.end());
  return str;
}
