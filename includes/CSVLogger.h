#ifndef CSV_LOGGER
#define CSV_LOGGER
#include <fstream>
#include <vector>
#include <map>
#include <dlib/image_processing.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "ellipse.h"
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
    CSVLogger(std::string filename);
    void newRow();
    void addToRow(std::string key, std::string value);
    void addToRow(std::string r_name, dlib::rectangle value);
    void addToRow(dlib::full_object_detection shape);
    void addToRow(std::string, cv::Point p);
	  void addToRow(EllipseROI eroi);
    void flush();
};
#endif
