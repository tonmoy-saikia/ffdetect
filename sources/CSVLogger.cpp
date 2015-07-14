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
  header_order.push_back("seek_time");
  header_order.push_back("face_rect_x");
  header_order.push_back("face_rect_y");
  header_order.push_back("face_rect_w");
  header_order.push_back("face_rect_h");
  header_order.push_back("eye_rect_x");
  header_order.push_back("eye_rect_y");
  header_order.push_back("eye_rect_w");
  header_order.push_back("eye_rect_h");
  header_order.push_back("nose_rect_x");
  header_order.push_back("nose_rect_y");
  header_order.push_back("nose_rect_w");
  header_order.push_back("nose_rect_h");
  header_order.push_back("mouth_rect_x");
  header_order.push_back("mouth_rect_y");
  header_order.push_back("mouth_rect_w");
  header_order.push_back("mouth_rect_h");
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

void CSVLogger::addToRow(cv::Point p)
{
  addToRow("marker_coord_x", Utils::toString(p.x));
  addToRow("marker_coord_y", Utils::toString(p.y));
}

void CSVLogger::addToRow(std::string rType, cv::Rect value)
{
  std::vector<std::string> rectTypes;
  rectTypes.push_back("face"); rectTypes.push_back("nose");
  rectTypes.push_back("mouth"); rectTypes.push_back("eye");
  std::vector<std::string>::iterator it = std::find(rectTypes.begin(), rectTypes.end(), rType);
  if(it != rectTypes.end())
  {
    addToRow(*it + "_rect_x", Utils::toString(value.x));
    addToRow(*it + "_rect_y", Utils::toString(value.y));
    addToRow(*it + "_rect_w", Utils::toString(value.width));
    addToRow(*it + "_rect_h", Utils::toString(value.height));
  }
  else
  {
    //raise exception
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
