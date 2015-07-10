CC=g++
CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`

all:
	g++ `pkg-config --cflags opencv` main.cpp sources/config.cpp sources/FaceFeatures.cpp sources/CSVLogger.cpp `pkg-config --libs opencv` -o main
