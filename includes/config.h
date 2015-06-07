#ifndef CONFIG_H
#define CONFIG_H
#include <string>
namespace config
{
  extern float SCALE_FACTOR;
  extern std::string HAARCASCADE_FACE;
  extern std::string HAARCASCADE_MOUTH;
  extern std::string HAARCASCADE_NOSE;
  extern std::string HAARCASCADE_EYES;
  bool load_config_file();
}

#endif
