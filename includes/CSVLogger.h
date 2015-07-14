#include "common.h"
#include <fstream>

class CSVLogger
{
  protected:
    std::ofstream fobj;
    std::vector< std::map<std::string, std::string> > rows;
    std::map<std::string, std::string> default_row;
    std::vector<std::string> header_order;
    void create_default_row();
    void set_header_order();
  public:
    CSVLogger(const char*);
    void newRow();
    void addToRow(std::string key, std::string value);
    void addToRow(std::string rectType, cv::Rect value);
    void addToRow(cv::Point p);
    void flush();
};
