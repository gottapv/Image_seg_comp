/*


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;


void compute_skew(const char* filename)
{
	// Load in grayscale.
	cv::Mat img = cv::imread(filename, 0);

	// Binarize
	cv::threshold(img, img, 225, 255, cv::THRESH_BINARY);

	// Invert colors
	cv::bitwise_not(img, img);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 3));
	cv::erode(img, img, element);
	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = img.begin<uchar>();
	cv::Mat_<uchar>::iterator end = img.end<uchar>();
	for (; it != end; ++it)
		if (*it)
			points.push_back(it.pos());
	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;
	cv::Point2f vertices[4];
	box.points(vertices);
	for (int i = 0; i < 4; ++i)
		cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);

	std::cout << "File " << filename << ": " << angle << std::endl;
	cv::imshow("Result", img);
	cv::waitKey(0);
}

void main(){
	
	compute_skew("MinuteMaid-Top.png");

}

*/

/*

//#include "stdafx.h"
//#include <opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	Mat im1 = imread("MinuteMaid-Top.PNG", 0);
	Mat im2 = imread("MinuteMaid-Top2.PNG", 0);

	Mat r1;
	im1.convertTo(r1, CV_64F);
	Mat r2;
	im2.convertTo(r2, CV_64F);

	Point2d phaseShift;

	if (r1.cols != r2.cols || r1.rows != r2.rows)
	{
		int n_cols = max(r1.cols, r2.cols);
		int n_rows = max(r1.rows, r2.rows);

		Mat r1_pad;
		copyMakeBorder(r1, r1_pad, 0, n_rows - r1.rows, 0, n_cols - r1.cols, BORDER_CONSTANT, Scalar::all(0));
		Mat r2_pad;
		copyMakeBorder(r2, r2_pad, 0, n_rows - r2.rows, 0, n_cols - r2.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat hann;
		createHanningWindow(hann, r1_pad.size(), CV_64F);
		phaseShift = phaseCorrelate(r1_pad, r2_pad, hann);
	}
	else
	{
		Mat hann;
		createHanningWindow(hann, r1.size(), CV_64F);
		phaseShift = phaseCorrelate(r1, r2, hann);
	}


	imshow("r1", r1);
	imshow("r2", r2);
	waitKey(0);
		return 0;

}


*/

/*
* Copyright (c) 2012. Philipp Wagner <bytefish[at]gmx[dot]de>.
* Released to public domain under terms of the BSD Simplified license.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of the organization nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*   See <http://www.opensource.org/licenses/bsd-license>
*/

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

// Normalizes a given image into a value range between 0 and 255.
Mat norm_0_255(const Mat& src) {
	// Create and return normalized image:
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}

//
//void Feature::makeFeature(cv::Mat &image, cv::Mat &result)
//{
//	cv::resize(image, image, cv::Size(), 0, 0, cv::INTER_CUBIC);
//	cv::equalizeHist(image, image);
//	// Images must be aligned - Only pitch executed, yaw and roll assumed   negligible
//	algmt->getAlignedImage(image, image); // image alignment
//	// tan triggs
//	{
//		tan_triggs_preprocessing(image, result);
//		result = result.reshape(0, 1); // make a single row vector, needed for the training samples matrix
//	}
//	// if plain intensity
//{
//	//    image.copyTo(result);
//	//    result.convertTo(result, CV_32F, 1.0f/255.0f);
//	//    result = result.reshape(0, 1); // make a single row vector, needed for the training samples matrix
//}
//}

//
// Calculates the TanTriggs Preprocessing as described in:
//
//      Tan, X., and Triggs, B. "Enhanced local texture feature sets for face
//      recognition under difficult lighting conditions.". IEEE Transactions
//      on Image Processing 19 (2010), 1635–650.
//
// Default parameters are taken from the paper.
//
Mat tan_triggs_preprocessing(InputArray src,
	float alpha = 0.1, float tau = 10.0, float gamma = 0.2, int sigma0 = 1,
	int sigma1 = 2) {

	// Convert to floating point:
	Mat X = src.getMat();
	X.convertTo(X, CV_32FC1);
	// Start preprocessing:
	Mat I;
	pow(X, gamma, I);
	// Calculate the DOG Image:
	{
		Mat gaussian0, gaussian1;
		// Kernel Size:
		int kernel_sz0 = (3 * sigma0);
		int kernel_sz1 = (3 * sigma1);
		// Make them odd for OpenCV:
		kernel_sz0 += ((kernel_sz0 % 2) == 0) ? 1 : 0;
		kernel_sz1 += ((kernel_sz1 % 2) == 0) ? 1 : 0;
		GaussianBlur(I, gaussian0, Size(kernel_sz0, kernel_sz0), sigma0, sigma0, BORDER_CONSTANT);
		GaussianBlur(I, gaussian1, Size(kernel_sz1, kernel_sz1), sigma1, sigma1, BORDER_CONSTANT);
		subtract(gaussian0, gaussian1, I);
	}

	{
		double meanI = 0.0;
		{
			Mat tmp;
			pow(abs(I), alpha, tmp);
			meanI = mean(tmp).val[0];

		}
		I = I / pow(meanI, 1.0 / alpha);
	}

	{
		double meanI = 0.0;
		{
			Mat tmp;
			pow(min(abs(I), tau), alpha, tmp);
			meanI = mean(tmp).val[0];
		}
		I = I / pow(meanI, 1.0 / alpha);
	}

	// Squash into the tanh:
	{
		for (int r = 0; r < I.rows; r++) {
			for (int c = 0; c < I.cols; c++) {
				I.at<float>(r, c) = tanh(I.at<float>(r, c) / tau);
			}
		}
		I = tau * I;
	}
	return I;
}

int main(int argc, const char *argv[]) {
	// Get filename to the source image:
	/*if (argc != 2) {
		cout << "usage: " << argv[0] << " <image.ext>" << endl;
		exit(1);
	}*/
	// Load image & get skin proportions:
	Mat image = imread("MinuteMaid-Top2.png", CV_LOAD_IMAGE_GRAYSCALE);
	// Calculate the TanTriggs Preprocessed image with default parameters:
	Mat preprocessed = tan_triggs_preprocessing(image);
	// Draw it on screen:
	imshow("Original Image", image);
	imshow("TanTriggs Preprocessed Image", norm_0_255(preprocessed));
	// Show the images:
	waitKey(0);
	// Success!
	return 0;
}