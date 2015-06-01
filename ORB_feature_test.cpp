#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
//#include "opencv2/nonfree/features2d.hpp"

using namespace cv;

void readme();

/**
* @function main
* @brief Main function
*/
int main(int argc, char** argv)
{
	/*if (argc != 3)
	{
		readme(); return -1;
	}*/

	Mat img_1 = imread("MinuteMaid-Top.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_2 = imread("MinuteMaid-Top.png", CV_LOAD_IMAGE_GRAYSCALE);

	/*if (!img_1.data || !img_2.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return -1;
	}
	*/
	//-- Step 1: Detect the keypoints using SURF Detector
 	int minHessian = 400;

	//FastFeatureDetector;
	
	FeatureDetector detector;

	std::vector<KeyPoint> keypoints_1, keypoints_2;

	detector.detect(img_1, keypoints_1);
	detector.detect(img_2, keypoints_2);

	//-- Step 2: Calculate descriptors (feature vectors)
	//SurfDescriptorExtractor extractor;
	DescriptorExtractor extractor;

	Mat descriptors_1, descriptors_2;

	extractor.compute(img_1, keypoints_1, descriptors_1);
	extractor.compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- Draw only "good" matches
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	imshow("Good Matches", img_matches);

	for (int i = 0; i < (int)good_matches.size(); i++)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}

	waitKey(0);

	return 0;
}

/**
* @function readme
*/
void readme()
{
	std::cout << " Usage: ./SURF_FlannMatcher <img1> <img2>" << std::endl;
}
/*
{

	Mat img1 = imread("MinuteMaid-Top.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("MinuteMaid-Top.png", CV_LOAD_IMAGE_GRAYSCALE);

	KeyPoint keypoints1; 
	KeyPoint keypoints2;
	Mat descriptors1;
	Mat descriptors2;

	//Definition of ORB keypoint detector and descriptor extractors
	FeatureDetector detector = ;
	descripto
	DescriptorExtractor extractor = DescriptorExtractor.create(DescriptorExtractor.ORB);

	//Detect keypoints
	detector.detect(img1, keypoints1);
	detector.detect(img2, keypoints2);
	//Extract descriptors
	extractor.compute(img1, keypoints1, descriptors1);
	extractor.compute(img2, keypoints2, descriptors2);

	//Definition of descriptor matcher
	DescriptorMatcher matcher = DescriptorMatcher.create(DescriptorMatcher.BRUTEFORCE_HAMMING);

	//Match points of two images
	MatOfDMatch matches = new MatOfDMatch();
	matcher.match(descriptors1, descriptors2, matches);


}
*/