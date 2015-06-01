#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <limits>
#include <queue>

using namespace cv;
using namespace std;

Mat img; Mat templ; Mat result;
string image_window = "Source Image";
string result_window = "Result window";

int match_method;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod(int, void*);

struct seg_coords
{
	int minX, minY, maxX, maxY;
};

struct seg_Type{
	Mat segment;
	Point2d segment_coords;
	bool found_match = false;
};


void maxLocs(const Mat& src, queue<Point>& dst, size_t size)
{
	float maxValue = -1.0f * numeric_limits<float>::max();
	float* srcData = reinterpret_cast<float*>(src.data);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (srcData[i*src.cols + j] > maxValue)
			{
				maxValue = srcData[i*src.cols + j];

				dst.push(Point(j, i));

				// pop the smaller one off the end if we reach the size threshold.
				if (dst.size() > size)
				{
					dst.pop();
				}
			}
		}
	}
}

int main(int argc, char** argv)
{

	Mat load_img = imread("MinuteMaid-Top.png");
	Mat load_img2 = imread("MinuteMaid-Top2.png");

	vector<seg_Type> list_Segments; //img2 segments list
	seg_Type temp_seg_type;

	Mat sub_block_temp1, sub_block_temp2;

	/*
	for (int i = 0; i < load_img2.cols-100; i += 100){
	for (int j = 0; j < load_img2.rows-100; j += 100){
	temp_seg_type.segment = load_img2(Rect(i, j, 100, 100));
	temp_seg_type.segment_coords.x = i; temp_seg_type.segment_coords.y = j;
	list_Segments.push_back(temp_seg_type);
	}
	}
	*/

	cout << "rows" << load_img2.rows << "cols" << load_img2.cols << "size" << load_img2.size() << endl;

	img = load_img;
	templ = load_img(Rect(400, 500, 100, 100));
	//	templ = list_Segments.at(5).segment;

	/// Create windows
	namedWindow(image_window, CV_WINDOW_AUTOSIZE);
	namedWindow(result_window, CV_WINDOW_AUTOSIZE);

	/// Create Trackbar
	string trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);

	MatchingMethod(0, 0);

	//	imshow("Loaded Image 2", load_img2);

	waitKey(0);

	return 0;

}

void MatchingMethod(int, void*)
{
	/// Source image to display
	Mat img_display;
	img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_cols, result_rows, CV_32FC1);

	/// Do the Matching and Normalize
	cvMatchTemplate(&img, &templ, &result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		result = 1.0 - result;
	}

	// get the top 100 maximums...
	queue<Point> locations;
	maxLocs(result, locations, 100);

	/// Show me what you got
	while (!locations.empty())
	{
		Point matchLoc = locations.front();
		cvRectangle(&img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		cvRectangle(&result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		locations.pop();
	}

	imshow(image_window, img_display);
	imshow(result_window, result);

	return;
}