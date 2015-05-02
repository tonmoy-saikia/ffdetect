#include "common.h"

class Face
{
private:
  cv::CascadeClassifier face;
public:
  Face();
  std::vector<cv::Rect> detect(cv::Mat im);
  static void drawRectangle();
};

