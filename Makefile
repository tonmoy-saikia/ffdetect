#http://mrbook.org/blog/tutorials/make/
CC=g++
#CFLAGS=-c -Wall
CFLAGS = `pkg-config --cflags opencv`
LDFLAGS= `pkg-config --libs opencv`
#SOURCES=main.cpp sources/config.cpp sources/CSVLogger.cpp sources/FaceFeatures.cpp sources/Utils.cpp

all:
	$(CC) $(CFLAGS) main.cpp sources/FaceFeatures.cpp sources/config.cpp sources/CSVLogger.cpp sources/Utils.cpp $(LDFLAGS) -o ffdetect

