#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

using namespace cv;
using namespace std;


int main(int argc, char **argv)
{
	IplImage *img1;
	IplImage *img2;
	IplImage *imggray1;
	IplImage *imggray2;
	IplImage *imggray3;
	
		img1 = cvLoadImage("MinuteMaid-Top.png");
		img2 = cvLoadImage("MinuteMaid-Top2.png");
	

	IplImage *imgsize1, *imgsize2;
	//imgsize1 = cvSize( cvGetSize(img1));
	//imgsize2 = cvSize( img2 );
	//if (!(imgsize1 && imgsize2)) return -1;

	imggray1 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	imggray2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
	imggray3 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);

	int key = 0, c;


		// convert rgb to grayscale
		cvCvtColor(img1, imggray1, CV_RGB2GRAY);

		// quit if user press ‘q’ and wait a bit between images
		key = cvWaitKey(500);

		// convert rgb to grayscale
		cvCvtColor(img2, imggray2, CV_RGB2GRAY);

		// compute difference
		cvAbsDiff(imggray1, imggray2, imggray3);

		// display difference
		cvNamedWindow("image", 1);
		cvShowImage("image", imggray3);
	
		waitKey(0);

	// release camera and clean up resources when “q” is pressed
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvDestroyWindow("image");
	return 0;

}

