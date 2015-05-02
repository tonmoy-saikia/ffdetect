#include "../includes/config.h"


//http://stackoverflow.com/questions/10892167/proper-implementation-of-global-configuration


namespace config
{
  float SCALE_FACTOR = 1.3;
  std::string config_str = "sss";
}

bool config::load_config_file()
{
      // Code to load and set the configuration variables
      SCALE_FACTOR = 2.0;
}
