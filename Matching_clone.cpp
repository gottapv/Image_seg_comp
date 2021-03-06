#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <sstream>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img; Mat templ; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method = 3;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod(int, void*);

/**
* @function main
*/
int main(int, char** argv)
{
	/// Load image and template
	img = imread("C:/Insigma_OpenCV/Test_Project/Image1_Segments/segment_36.png", 1); //("segment_36_big_seg_skew.png", 1); //
	templ = imread("segment_36_seg_skew.png", 1); //("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_36.png", 1);  //
	//img = imread("C:/Insigma_OpenCV/Test_Project/Image1_Segments/segment_36.png", 1);
	//templ = imread("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_36.png", 1);


	/// Create windows
	namedWindow(image_window, WINDOW_AUTOSIZE);
	//	namedWindow(result_window, WINDOW_AUTOSIZE);


	/*
	Mat i1 = imread("segment_36_big_seg_skew.png");
	Mat i2 = imread("segment_36_seg_skew.png");
	Mat i3 = imread("C:/Insigma_OpenCV/Test_Project/Image1_Segments/segment_36.png");
	Mat i4 = imread("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_36.png");


	imshow("To be compared to Segment", i3);
	imshow("To be compared to Segment Skewed", i2);
	imshow("Segment small", i4);
	imshow("Segment small skewed", i1);*/

	/// Create Trackbar
	//const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	//createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);

	MatchingMethod(0, 0);

	waitKey(0);
	return 0;
}

/**
* @function MatchingMethod
* @brief Trackbar callback
*/
void MatchingMethod(int, void*)
{
	/// Source image to display
	Mat img_display;
	img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
	//	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	//	cv::threshold(result, result, 0.8, 1., CV_THRESH_TOZERO);

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	double thres = 0.8;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/*/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
	matchLoc = minLoc;
	}
	else
	{
	}*/

	matchLoc = maxLoc;

	/// Show me what you got

	if (maxVal >= thres){
		rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		//	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		//	imshow(result_window, result);
		imshow(image_window, img_display);
	}

	return;
}
