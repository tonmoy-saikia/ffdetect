#include "../includes/CSVLogger.h"
#include "../includes/Utils.h"
#include <algorithm>

CSVLogger::CSVLogger(const char* filename)
{
  fobj.open(filename);
  create_default_row();
}

void CSVLogger::create_default_row()
{
  default_row["frame_no"]="";
  default_row["seek_time"]="";
  default_row["face_rect_x"]="";
  default_row["face_rect_y"]="";
  default_row["face_rect_w"]="";
  default_row["face_rect_h"]="";
  default_row["eye_rect_x"]="";
  default_row["eye_rect_y"]="";
  default_row["eye_rect_w"]="";
  default_row["eye_rect_h"]="";
  default_row["nose_rect_x"]="";
  default_row["nose_rect_y"]="";
  default_row["nose_rect_w"]="";
  default_row["nose_rect_h"]="";
  default_row["mouth_rect_x"]="";
  default_row["mouth_rect_y"]="";
  default_row["mouth_rect_w"]="";
  default_row["mouth_rect_h"]="";
  default_row["marker_loc"]="";
  default_row["marker_coord_x"]="";
  default_row["marker_coord_y"]="";
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
  rectTypes.push_back("mouth"); rectTypes.push_back("eyes");
  std::vector<std::string>::iterator it = std::find(rectTypes.begin(), rectTypes.end(), rType);
  if(it != rectTypes.end())
  {
    addToRow(*it + "_rect_x", Utils::toString(value.x));
    addToRow(*it + "_rect_y", Utils::toString(value.x));
    addToRow(*it + "_rect_w", Utils::toString(value.x));
    addToRow(*it + "_rect_h", Utils::toString(value.x));
  }
  else
  {
    //raise exception
  }
}

void CSVLogger::flush()
{
  std::string val, out_str;
  for (int i=0; i<rows.size();i++)
  {
    std::map<std::string, std::string> row = rows[i];
    for(std::map<std::string, std::string>::iterator iter = row.begin(); iter!=row.end(); ++iter)
    {
      val = iter->second;
      if(iter == row.begin())
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
  }
  fobj.close();
}
