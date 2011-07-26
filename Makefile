##########################################################################

# OpenCV Teaching Examples

# Author : Toby Breckon, toby.breckon@cranfield.ac.uk

# Copyright (c) 2006 School of Engineering, Cranfield University
# License : LGPL - http://www.gnu.org/licenses/lgpl.html

##########################################################################

# opencv setup using pkg-config

OPENCV_INCLUDE=`pkg-config opencv --cflags`
OPENCV_LIB=`pkg-config opencv --libs` # -L/opt/opencv/ffmpeg/lib -lavcodec -lavformat -lavutil

# general compiler setup

CC=gcc
CC_OPTS=-O3 -Wall

##########################################################################

EXAMPLES=adapthistequal

##########################################################################

all:
	make $(EXAMPLES)


##########################################################################

# cranfield MSc demos

adapthistequal: adapthistequal.cc clahe.cc $(OBJS) $(HEADERS)
	$(CC) $(CC_OPTS) $(CFLAGS) $(OPENCV_INCLUDE) $(OPENCV_LIB) \
	adapthistequal.cc clahe.cc $(OBJS) -o adapthistequal

##########################################################################

clean:
	rm -f *~ $(EXAMPLES)

##########################################################################
