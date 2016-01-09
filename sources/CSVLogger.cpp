#include "../includes/CSVLogger.h"
#include "../includes/Utils.h"
#include <algorithm>

CSVLogger::CSVLogger(const char* filename)
{
  fobj.open(filename);
  set_header_order();
  create_default_row();
}

void CSVLogger::set_header_order()
{
  header_order.push_back("frame_no");
  header_order.push_back("ts");
  header_order.push_back("face_rect_x1");
  header_order.push_back("face_rect_y1");
  header_order.push_back("face_rect_x2");
  header_order.push_back("face_rect_y2");

  for(int i=1; i<=68; i++)
  {
    std::string part_name = "l" + Utils::toString(i);
    header_order.push_back(part_name + "_x");
	  header_order.push_back(part_name + "_y");
  }
  std::string ellipse_types[] = {"elps_mouth", "elps_l_eye", "elps_r_eye", "elps_face"};
  for(int i=0; i<4; i++)
  {
    header_order.push_back(ellipse_types[i] + "_x");
    header_order.push_back(ellipse_types[i] + "_y");
  }

  header_order.push_back("marker_loc");
  header_order.push_back("marker_coord_x");
  header_order.push_back("marker_coord_y");
}

void CSVLogger::create_default_row()
{
  for(int i=0; i<header_order.size(); i++)
    default_row[header_order[i]] = "";
}

void CSVLogger::newRow()
{
  rows.push_back(default_row);
}

void CSVLogger::addToRow(std::string key, std::string value)
{
  std::map<std::string, std::string> *last_row;
  if(rows.size() == 0 )
    newRow();
  last_row = &rows.back();
  if((*last_row).count(key)>0)
  {
    (*last_row)[key] = value;
  }
  else
  {
    //raise exception
  }
}

void CSVLogger::addToRow(std::string name, cv::Point p)
{
  addToRow(name + "_x", Utils::toString(p.x));
  addToRow(name + "_y", Utils::toString(p.y));
}

void CSVLogger::addToRow(std::string r_name, dlib::rectangle value)
{
  addToRow(r_name + "_x1", Utils::toString(value.left()));
  addToRow(r_name + "_y1", Utils::toString(value.top()));
  addToRow(r_name + "_x2", Utils::toString(value.right()));
  addToRow(r_name + "_y2", Utils::toString(value.bottom()));
}


void CSVLogger::addToRow(dlib::full_object_detection shape)
{
  for(int i=0; i<shape.num_parts();i++)
  {
    std::string lx = "l" + Utils::toString(i+1) + "_x";
	std::string ly = "l" + Utils::toString(i+1) + "_y";
	addToRow(lx, Utils::toString(shape.part(i).x()));
	addToRow(ly, Utils::toString(shape.part(i).y()));
  }
}

void CSVLogger::addToRow(EllipseROI eroi)
{
	addToRow("elps_" + eroi.type, eroi.center);
}
void CSVLogger::flush()
{
  std::string val, out_str;
  for(int i=0; i<=rows.size(); i++)
  {
    for(int j=0; j<header_order.size(); j++)
    {
      if(i>0)
      {
        val = rows[i-1][header_order[j]];
      }
      else
      {
        val = header_order[j];
      }
      if(j==0)
      {
        out_str.append(val);
      }
      else
      {
        out_str.append(",");
        out_str.append(val);
      }
    }
    out_str.append("\n");
    fobj << out_str;
    out_str ="";
  }
  fobj.close();
}
