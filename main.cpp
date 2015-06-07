#include "includes/common.h"
#include "includes/FaceFeatures.h"


void extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect);
int main(int argc, const char** argv)
{
  if( argc != 2)
  {
    std::cout <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
    return -1;
  }

  Face f; Nose n; Eyes e; Mouth m;

  cv::Mat image;
  image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  std::vector<cv::Rect> faces = f.detect(image, cv::Size(100,100));

  if(faces.size()==1){
    cv::Rect faceRect = faces[0];
    //draw rectangle around face
    rectangle(image, faceRect, cv::Scalar(0,255,0),1,8,0);

    cv::Mat faceROI = image(faceRect);

    // detect nose
    //cv::Rect noseRect = n.detect(faceROI, cv::Size(40,40))[0];
    //extendRectangle(image, faceRect, noseRect);

    //detect mouth
    cv::Rect mouthRect = m.detect(faceROI, cv::Size(40,40))[0];
    extendRectangle(image, faceRect, mouthRect);

   //detect eyes
    cv::Rect eyeRect = e.detect(faceROI, cv::Size(40,40))[0];
    extendRectangle(image, faceRect, eyeRect);
  }
  else{
    std::cout<< "More than one faces found!";
    //log error
  }

  namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  imshow("Display window", image);
  cv::waitKey(0);
  return 0;
}

void extendRectangle(cv::Mat image, cv::Rect origRect, cv::Rect subRect)
{
  cv::Point pt1(origRect.x + subRect.x, origRect.y + subRect.y);
  cv::Point pt2(pt1.x + subRect.width, pt1.y + subRect.height);
  rectangle(image, pt1, pt2, cv::Scalar(255,0,0),1,8,0);
}
