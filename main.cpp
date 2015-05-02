#include "includes/common.h"
#include "includes/face.h"

int main(int argc, const char** argv)
{
  if( argc != 2)
  {
    std::cout <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
    return -1;
  }
  Face f;
  cv::Mat image;
  image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  rectangle(image, f.detect(image)[0], cv::Scalar(255,0,0),1,8,0);
  //cout<< f.detect(image).size();

  namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  imshow( "Display window", image );
  cv::waitKey(0);
  return 0;
}
