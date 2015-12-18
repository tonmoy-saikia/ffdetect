#ifndef LANDMARKMAPPER
#define LANDMARKMAPPER
#include<map>
#include<string>
#include <dlib/image_processing.h>
#include <opencv2/imgproc/imgproc.hpp>
typedef cv::Point_<float> pos_vec;

class Axes{
  public:
    Axes();
    pos_vec maj;
    pos_vec min;
};


class LandmarkMapper{
  private:
    dlib::full_object_detection shape;
    std::vector< cv::Point_<float> > get_points(int start_idx, int end_idx);
    Axes get_axes(std::string, cv::Point_<float>, cv::Point_<float>);

  public:
    LandmarkMapper();
    LandmarkMapper(dlib::full_object_detection shape);
    std::map<std::string, std::vector<cv::Point_<float> > > pmap;
    std::map<std::string, cv::Point_<float> > cmap;
    std::map<std::string, Axes > pvmap;
};
#endif
