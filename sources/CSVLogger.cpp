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
  header_order.push_back("face_rect");
  for(int i=1; i<=68; i++)
  {
    std::string part_name = "l" + Utils::toString(i);
    header_order.push_back(part_name);
  }
  header_order.push_back("marker_loc");
  header_order.push_back("marker_coord");
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

void CSVLogger::addToRow(cv::Point p)
{
  addToRow("marker_coord_x", Utils::toString(p.x));
  addToRow("marker_coord_y", Utils::toString(p.y));
}

void CSVLogger::addToRow(std::string r_name, dlib::rectangle value)
{
    std::string val = Utils::toString(value.left()) + ";"+
                      Utils::toString(value.top()) + ";" +
                      Utils::toString(value.right()) + ";"+
                      Utils::toString(value.bottom());
    addToRow(r_name, val);
}


void CSVLogger::addToRow(dlib::full_object_detection shape)
{
  for(int i=0; i<shape.num_parts();i++)
  {
    std::string key = "l" + Utils::toString(i+1);
    std::string val = Utils::toString(shape.part(i).x()) + ";" + Utils::toString(shape.part(i).y());
    addToRow(key, val);
  }
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
