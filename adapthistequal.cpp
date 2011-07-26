// Example : adaptive histogram equalise grayscale image
// usage: prog {<image_name> | <video_name>}

// Author : Toby Breckon, toby.breckon@cranfield.ac.uk

// Copyright (c) 2009 School of Engineering, Cranfield University
// License : LGPL - http://www.gnu.org/licenses/lgpl.html

#include "cv.h"       // opencv general include file
#include "highgui.h"  // opencv GUI include file

#include "clahe.h"	  		  // opencv CLAHE include file
#include "useful_macros.h"	  // opencv macros include file

#include <stdio.h>

/******************************************************************************/
// setup the camera index properly based on OS platform

// 0 in linux gives first camera for v4l
//-1 in windows gives first device or user dialog selection

#ifdef linux
	#define CAMERA_INDEX 0
#else
	#define CAMERA_INDEX -1
#endif

/******************************************************************************/

// function that takes a gray scale image and draws a histogram 
// image for it in a pre-allocated image

void create_histogram_image(IplImage* grayImg, IplImage* histogramImage){

  CvHistogram *hist = NULL;	    // pointer to histogram object
  float max_value = 0;			// max value in histogram
  int hist_size = 256;			// size of histogram (number of bins)
  int bin_w = 0;				// initial width to draw bars
  float range_0[]={0,256};
  float* ranges[] = { range_0 };	

  hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  
  cvCalcHist( &grayImg, hist, 0, NULL );
  cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
  cvScale( hist->bins, hist->bins, ((double)histogramImage->height)/max_value, 0 );
  cvSet( histogramImage, cvScalarAll(255), 0 );
  bin_w = cvRound((double)histogramImage->width/hist_size);

  for(int i = 0; i < hist_size; i++ )
  {
     cvRectangle( histogramImage, cvPoint(i*bin_w, histogramImage->height),
                  cvPoint((i+1)*bin_w, histogramImage->height 
	  								- cvRound(cvGetReal1D(hist->bins,i))),
                   					cvScalarAll(0), -1, 8, 0 );
  }
  
  cvReleaseHist (&hist);
}	

/******************************************************************************/

int main( int argc, char** argv )
{

  IplImage* img = NULL;      // image object 
  CvCapture* capture = NULL; // capture object
	
  IplImage* eqHistogramImage = NULL;  	// histogram images
  IplImage* grayHistogramImage = NULL;	
	
  const char* windowName = "CL Adaptive Histogram Equalization"; // window name
  const char* windowName1 = "Grayscale"; // window name
  const char* windowNameH1 = "Adaptive Equalised Histogram"; // window name
  const char* windowNameH2 = "Original Histogram"; // window name
	
  bool keepProcessing = true;	// loop control flag
  char key;						// user input	
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame		
	
  int xdivs = 2;
  int ydivs = 2;
  int bins = 256;
  int limit_counter = 14;	
	
  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera 

    if( 
	  ( argc == 2 && (img = cvLoadImage( argv[1], 1)) != 0 ) ||
	  ( argc == 2 && (capture = cvCreateFileCapture( argv[1] )) != 0 ) || 
	  ( argc != 2 && (capture = cvCreateCameraCapture( 0 )) != 0 )
	  )
    {
      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      cvNamedWindow(windowName, 1);		// flag set to 1 by Shervin Emami, 17Nov2010.
	  cvNamedWindow(windowName1, 1);	// flag set to 1 by Shervin Emami, 17Nov2010.
	  cvNamedWindow(windowNameH1, 1);	// flag set to 1 by Shervin Emami, 17Nov2010.
	  cvNamedWindow(windowNameH2, 1);	// flag set to 1 by Shervin Emami, 17Nov2010.
		
      cvNamedWindow("Simple Histogram Equalization", 1);		// Added by Shervin Emami, 17Nov2010.

	  cvCreateTrackbar("X cells", windowName, &xdivs, 16, NULL);				
	  cvCreateTrackbar("Y cells", windowName, &ydivs, 16, NULL);
	  cvCreateTrackbar("bins", windowName, &bins, 256, NULL);
	  cvCreateTrackbar("limit (x 0.1)", windowName, &limit_counter, 30, NULL);
		
	  // define required images for intermediate processing
      // (if using a capture object we need to get a frame first to get the size)
		
	  if (capture) {
	
		  // cvQueryFrame s just a combination of cvGrabFrame 
		  // and cvRetrieveFrame in one call.
		  
		  img = cvQueryFrame(capture);
		  if(!img){
			if (argc == 2){				  
				printf("End of video file reached\n");
			} else {
				printf("ERROR: cannot get next fram from camera\n");
			}
			exit(0);
		  }
		  
	  }
	  
	  IplImage* grayImg = 
	  			cvCreateImage(cvSize(img->width,img->height), img->depth, 1);
	  grayImg->origin = img->origin;
	  IplImage* eqImg = 
	  			cvCreateImage(cvSize(img->width,img->height), img->depth, 1);
	  eqImg->origin = img->origin;		
		
	  eqHistogramImage = cvCreateImage(cvSize(255,200), 8, 1);
	  grayHistogramImage = cvCreateImage(cvSize(255,200), 8, 1);
	  
	  // start main loop	
		
	  while (keepProcessing) {
		
		  // if capture object in use (i.e. video/camera)
		  // get image from capture object
			
		  if (capture) {
	
			  // cvQueryFrame s just a combination of cvGrabFrame 
			  // and cvRetrieveFrame in one call.
			  
			  img = cvQueryFrame(capture);
			  if(!img){
				if (argc == 2){				  
					printf("End of video file reached\n");
				} else {
					printf("ERROR: cannot get next fram from camera\n");
				}
				exit(0);
			  }
			  
		  }	else {
			  
			  // if not a capture object set event delay to zero so it waits
			  // indefinitely (as single image file, no need to loop)
			  
			  EVENT_LOOP_DELAY = 0;
		  }			  
		
		  // *** Histogram Equalisation Processing
		  
			  // if input is not already grayscale, convert to grayscale
			
			  if (img->nChannels > 1){
				  cvCvtColor(img, grayImg, CV_BGR2GRAY);
			  } else {
				  grayImg = img;
			  }
		   
			  // histogram equalize it (checking first for valid cells numbers)
//			  if ((img->width % xdivs) || (img->height % ydivs))
			  if (0)	// Check has been removed by Shervin Emami, 17Nov2010.
			  {
				printf("X cells and Y cells must be multiples of image height and image width\n\n");

			  } else {
  			    CV_TIMER_START(X)  
 				cvCLAdaptEqualize(grayImg, eqImg, (unsigned int) xdivs, (unsigned int) ydivs, 
					(unsigned int) bins, (float) limit_counter * 0.1, CV_CLAHE_RANGE_FULL);
			  	CV_TIMER_STOP(X, "CLAHE")
			  }
			  
			  
		  // ***
		  
	      // *** draw histograms 	  
			
		  create_histogram_image(grayImg, grayHistogramImage);
		  create_histogram_image(eqImg, eqHistogramImage);		  
			  
		  // display image in window
	
		  cvShowImage( windowName,  eqImg );
		  cvShowImage( windowName1, grayImg );
		  
		  cvShowImage( windowNameH1,  eqHistogramImage );
		  cvShowImage( windowNameH2, grayHistogramImage );	  

		  // Simple Histogram Equalization. Added by Shervin Emami, 17Nov2010.
		  IplImage *histEqImage = cvCreateImage(cvGetSize(grayImg), grayImg->depth, grayImg->nChannels);
		  cvEqualizeHist(grayImg, histEqImage);
		  cvShowImage( "Simple Histogram Equalization",  histEqImage );
		  cvReleaseImage(&histEqImage);

		  // start event processing loop (very important,in fact essential for GUI)
	      // 4 ms roughly equates to 100ms/25fps = 4ms per frame
		  
		  key = cvWaitKey(EVENT_LOOP_DELAY);

		  if (key == 'x' || key == 27) {	// 'Esc' key added by Shervin Emami, 17Nov2010.
			
	   		// if user presses "x" or 'Esc' then exit
			
	   			printf("Keyboard exit requested : exiting now - bye!\n");	
	   			keepProcessing = false;
		  } 
	  }
      
      // destroy window objects
      // (triggered by event loop *only* window is closed)

      cvDestroyAllWindows();

      // destroy image object (if it does not originate from a capture object)

      if (!capture){
		  cvReleaseImage( &img );
      }
	  cvReleaseImage( &grayImg );
	  cvReleaseImage( &eqImg );
	  
	  // release histogram images
		  
	  cvReleaseImage( &eqHistogramImage );
      cvReleaseImage( &grayHistogramImage );
	  
      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
/******************************************************************************/
