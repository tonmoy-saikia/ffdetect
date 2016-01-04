#http://mrbook.org/blog/tutorials/make/
CC=g++
#CFLAGS=-c -Wall
CFLAGS = `pkg-config --cflags opencv`
LDFLAGS= `pkg-config --libs opencv`
#SOURCES=main.cpp sources/config.cpp sources/CSVLogger.cpp sources/FaceFeatures.cpp sources/Utils.cpp

all:
	$(CC) -O3 -I . $(CFLAGS) ./dlib/all/source.cpp main.cpp sources/LandmarkMapper.cpp sources/CSVLogger.cpp sources/ellipse.cpp sources/Utils.cpp $(LDFLAGS) -o ffdetectw

#g++ -O3 -I /home/tonmoy/my_repo/dlib-18.18/ /home/tonmoy/my_repo/dlib-18.18/dlib/all/source.cpp -lX11 lmark.cpp `pkg-config --libs opencv` -o testrun;
