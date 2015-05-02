#ifndef CONFIG_H
#define CONFIG_H
#include <string>
namespace config
{
  extern float SCALE_FACTOR;
  extern std::string config_str;
  bool load_config_file();
}

#endif
