// Header File of Useful OpenCV MACROS

// Author : Toby Breckon, toby.breckon@cranfield.ac.uk

// Copyright (c) 2009 School of Engineering, Cranfield University
// License : LGPL - http://www.gnu.org/licenses/lgpl.html

#include "cv.h"       // open cv general include file
#include "highgui.h"  // open cv GUI include file

#include <stdio.h>

//*****************************************************************************/

// simplified code timing using cvGetTickCount()

// Example usage:
// CV_START_TIMER(SOBEL)
// cvSobel(imgIn, imgOut, ....);
// CV_STOP_TIMER(SOBEL, "Sobel Time")

#define CV_TIMER_START(X)       double X = (double)cvGetTickCount();
#define CV_TIMER_STOP(X, STRING)       X = (double)cvGetTickCount() - X; \
                                            printf("Time @ [%s] = %gms\n", \
                                            STRING, X/((double)cvGetTickFrequency()*1000.) );

/******************************************************************************/

// simplified image display (displays an image and awaits a key press)

// Example usage: 
//
// DISPIMG_BREAKPOINT(image, "debug window - image 1");

#define DISPIMG_BREAKPOINT(I, WINDOWNAME) { cvNamedWindow(WINDOWNAME, 1 ); \
                                            cvShowImage(WINDOWNAME, I ); \
                                            cvWaitKey(0); \
                                            cvDestroyWindow(WINDOWNAME); \
                                          }

/******************************************************************************/

// simplified RGB colour comparison

#define COLOUR_COMP(r1,b1,g1,r2,b2,g2) ((r1 == r2) && (b1 == b2) && (g1 == g2))

/******************************************************************************/

// even more simplified pixel access (for 8-bit uchar images)

#define PIXEL(I, X, Y, C) (CV_IMAGE_ELEM((IplImage*) I, uchar, Y, (X * I->nChannels) + C))

/******************************************************************************/
