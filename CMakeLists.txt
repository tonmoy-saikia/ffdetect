cmake_minimum_required (VERSION 2.8)

PROJECT(ffdetect)
include_directories(./includes)

include("C:/Users/Tonmoy/dlib-18.18/dlib-18.18/dlib/cmake")
find_package( OpenCV REQUIRED )

ADD_EXECUTABLE(ffdetect main.cpp main.cpp sources/LandmarkMapper.cpp sources/CSVLogger.cpp sources/ellipse.cpp sources/Utils.cpp includes/LandmarkMapper.h includes/CSVLogger.h includes/ellipse.h includes/Utils.h)

TARGET_LINK_LIBRARIES(ffdetect dlib ${OpenCV_LIBS})