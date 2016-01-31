#include "../includes/LandmarkMapper.h"
#include "../includes/Utils.h"

Axes::Axes(){}
std::vector< cv::Point_<float> >LandmarkMapper::get_points(int start_idx, int end_idx)
{
  std::vector< cv::Point_<float> > points;
  for(int i=start_idx; i<=end_idx; i++)
    points.push_back(cv::Point_<float>(shape.part(i).x(), shape.part(i).y()));
  return points;
}

Axes LandmarkMapper::get_axes(std::string type, cv::Point_<float> max, cv::Point_<float> min)
{
  Axes ax;
  cv::Point_<float> center = cmap[type];
  ax.maj = Utils::get_pv(center,max);
  ax.min = Utils::get_pv(center,min);
  return ax;
}

LandmarkMapper::LandmarkMapper()
{
}

LandmarkMapper::LandmarkMapper(dlib::full_object_detection sh)
{
  shape = sh;
  pmap["mouth"] = get_points(48, 59);
  pmap["l_eye"] = get_points(36, 41);
  pmap["r_eye"] = get_points(42, 47);

  cmap["mouth"] = Utils::get_center_pt(pmap["mouth"]);
  cmap["l_eye"] = Utils::get_center_pt(pmap["l_eye"]);
  cmap["r_eye"] = Utils::get_center_pt(pmap["r_eye"]);
  cmap["face"] = (cv::Point_<float>(shape.part(1).x(), shape.part(1).y()) +
                 cv::Point_<float>(shape.part(15).x(), shape.part(15).y()))*0.5;


  cv::Point_<float> pt1 = cv::Point_<float>(shape.part(54).x(), shape.part(54).y());
  cv::Point_<float> pt2 = cv::Point_<float>(shape.part(13).x(), shape.part(13).y());
  //pvmap["mouth"] = get_axes( "mouth", (pt1 + pt2)*0.5,
                     //       cv::Point_<float>(shape.part(33).x(), shape.part(33).y()));
  Axes ax_m;
  ax_m.maj = Utils::get_pv(cv::Point_<float>(shape.part(48).x(), shape.part(48).y()), cv::Point_<float>(shape.part(54).x(), shape.part(54).y()))*0.65;
  ax_m.min = Utils::get_pv(cmap["mouth"], cv::Point_<float>(shape.part(33).x(), shape.part(33).y()));
  pvmap["mouth"] = ax_m;

  pvmap["l_eye"] = get_axes("l_eye", cv::Point_<float>(shape.part(27).x(), shape.part(27).y()),
                            cv::Point_<float>(shape.part(19).x(), shape.part(19).y()));

  pvmap["r_eye"] = get_axes("r_eye", cv::Point_<float>(shape.part(27).x(), shape.part(27).y()),
                            cv::Point_<float>(shape.part(24).x(), shape.part(24).y()));

  Axes ax;
  ax.maj = Utils::get_pv( cv::Point_<float>(shape.part(0).x(), shape.part(0).y()), cv::Point_<float>(shape.part(16).x(), shape.part(16).y()))*0.55;
  //ax.min = Utils::get_pv( cv::Point_<float>(shape.part(29).x(), shape.part(29).y()), cv::Point_<float>(shape.part(8).x(), shape.part(8).y()));
  ax.min = Utils::get_pv( cmap["face"], cv::Point_<float>(shape.part(8).x(), shape.part(8).y()));
  pvmap["face"] = ax ;

}

