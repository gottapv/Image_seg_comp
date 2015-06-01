

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

using namespace cv;
using namespace std;

//void drawMatches(const Mat& given_img, const vector<KeyPoint>& kp1, const Mat& img, const vector<KeyPoint>& kp, const vector<DMatch>& matches1to2, Mat& matchout, const Scalar& matchColor = Scalar::all(-1), const Scalar& singlePointColor = Scalar::all(-1), const vector<char>& matchesMask = vector<char>(), int flags = DrawMatchesFlags::DEFAULT);

const float inlier_threshold = 80.5f; // Distance threshold to identify inliers
const float nn_match_ratio = 0.8f;   // Nearest neighbor matching ratio

//Mat img_resize(Mat src_img, int new_width, int new_height)
//{
//	Mat des_img;
//	
//	resizeWindow(src_img, des_img, Size(new_width,new_height), 0, 0, CV_INTER_CUBIC);
//	
//	return des_img;
//}

/*
void rotate(cv::Mat& originalImage, cv::Mat& rotatedImage, cv::InputArray rotated,
	cv::Mat& dst) {
	std::vector<cv::Point2f> original(4);
	original[0] = cv::Point(0, 0);
	original[1] = cv::Point(originalImage.cols, 0);
	original[2] = cv::Point(originalImage.cols, originalImage.rows);
	original[3] = cv::Point(0, originalImage.rows);

	dst = cv::Mat::zeros(originalImage.rows, originalImage.cols, CV_8UC3);
	cv::Mat transform_img = cv::Mat::zeros(originalImage.rows, originalImage.cols, CV_8UC3);;
	transform_img =	cvGetPerspectiveTransform(rotated, original);
	cv::warpPerspective(rotatedImage, dst, transform, dst.size());

	
}
*/

Mat rotate_img(Mat& ref, Mat& disp){
//	Mat ref = faceIMGstore[1](faceROIstore[1]);
//	Mat disp = faceIMGstore[52](faceROIstore[52]);

	disp = disp.adjustROI(5, 5, 5, 5); //allow 5 pixel max adjustment in any direction
	if (disp.cols < ref.cols || disp.rows < ref.rows)
		cout << "Ref image small than disp image";

	Mat map;
	cvMatchTemplate(&disp, &ref, &map, CV_TM_SQDIFF_NORMED);


	Point minLoc;

	minMaxLoc(map, 0, &minLoc);

	Mat adjusted = disp(Rect(minLoc.x, minLoc.y, ref.cols, ref.rows));
	imshow("adjusted", adjusted);
	return adjusted;
}


int main(int argc, char** argv)
{

	Mat load_img = imread("MinuteMaid-Top.png");
	Mat load_img2 = imread("MinuteMaid-Top2.png");

	Mat given_img = load_img(Rect(400, 350, 700, 250));
	Mat given_img2 = load_img2(Rect(400, 350, 700, 250));

	//Mat given_img = img_resize(given_img_temp, 600, 400);

	//Mat given_img2 = img_resize(given_img2_temp, 600, 400);

	Mat rott_img;
	rott_img =	rotate_img(given_img, given_img2);

	Mat homography;
	FileStorage fs("H1to3p.xml", FileStorage::READ);
	fs.getFirstTopLevelNode() >> homography;

	//Mat sub_img = given_img(Rect(425, 400, 750, 200)); //imread("MinuteMaid-Top.png");
	//Mat sub_img2 = given_img2(Rect(425, 400, 750, 200));

//	std::vector<KeyPoint> kp;
//	std::vector<KeyPoint> kp_2;  //KP for sub images

	std::vector<KeyPoint> kp1;
	std::vector<KeyPoint> kp1_2;  //Kpfor given IMages

	Mat descriptor1, descriptor2;

	// Default parameters of ORB
	int nfeatures = 10000;
	float scaleFactor = 1.2f;
	int nlevels = 8;
	int edgeThreshold = 25; // Changed default (31);
	int firstLevel = 0;
	int WTA_K = 2;
	int scoreType = ORB::HARRIS_SCORE;
	int patchSize = 31;
	int fastThreshold = 15; // Changed default (20); ?? The less, the more KPs

	Ptr<ORB> detector = ORB::create(
		nfeatures,
		scaleFactor,
		nlevels,
		edgeThreshold,
		firstLevel,
		WTA_K,
		scoreType,
		patchSize,
		fastThreshold);

//	Ptr<ORB> detector1 = ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold);

	//Ptr<ORB> extractor = ORB;

//	detector->detect(sub_img, kp);
//	detector->detect(sub_img2, kp_2);  //detector for subimages

	detector->detect(given_img, kp1);	
	detector->detect(given_img2, kp1_2);

	Ptr<ORB> extractor = ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold);

	extractor->compute(given_img, kp1, descriptor1);
	extractor->compute(given_img2, kp1_2, descriptor2);

	std::cout << "Found " << kp1.size() << " Keypoints on Image1" << std::endl;
	std::cout << "Found " << kp1_2.size() << " Keypoints on Image2" << std::endl;


	/*
	Mat out1;
	drawKeypoints(sub_img, kp, out1, Scalar::all(255));
	Mat out2;
	drawKeypoints(sub_img2, kp_2, out2, Scalar::all(255));
	*/

	Mat fullout1;
	drawKeypoints(given_img, kp1, fullout1, Scalar(0,0,255));

	Mat fullout2;
	drawKeypoints(given_img2, kp1_2, fullout2, Scalar(0, 255, 0));

	Mat matchout;
	
	BFMatcher matcher(NORM_HAMMING, false);
	vector< vector<DMatch> > nn_matches;
	matcher.knnMatch(descriptor1, descriptor2, nn_matches, 2);

	vector<KeyPoint> matched1, matched2, inliers1, inliers2;
	vector<DMatch> good_matches;
	vector<DMatch> bad_matches;
	for (size_t i = 0; i < nn_matches.size(); i++) {
		DMatch first = nn_matches[i][0];
		float dist1 = nn_matches[i][0].distance;
		float dist2 = nn_matches[i][1].distance;

		if (dist1 < nn_match_ratio * dist2) {
			matched1.push_back(kp1[first.queryIdx]);
			matched2.push_back(kp1_2[first.trainIdx]);
		}
	}

	//Mat homography;

	for (unsigned i = 0; i < matched1.size(); i++) {
		Mat col = Mat::ones(3, 1, CV_64F);
		col.at<double>(0) = matched1[i].pt.x;
		col.at<double>(1) = matched1[i].pt.y;

		col = homography * col;
		col /= col.at<double>(2);
		double dist = sqrt(pow(col.at<double>(0) - matched2[i].pt.x, 2) +
			pow(col.at<double>(1) - matched2[i].pt.y, 2));

		if (dist < inlier_threshold) {
			int new_i = static_cast<int>(inliers1.size());
			inliers1.push_back(matched1[i]);
			inliers2.push_back(matched2[i]);
			good_matches.push_back(DMatch(new_i, new_i, 0));
		}
	/*	else{
			int new_i = static_cast<int>(inliers1.size());
			inliers1.push_back(matched1[i]);
			inliers2.push_back(matched2[i]);

			bad_matches.push_back(DMatch(new_i, new_i, 0));
		}*/
	}
	

		Mat res;

		drawMatches(given_img, inliers1, given_img2, inliers2, good_matches, res);
	//	drawMatches(given_img, inliers1, given_img2, inliers2, bad_matches, res);
		namedWindow("result", CV_WINDOW_NORMAL);
		resizeWindow("result", 820, 640);
		imshow("result", res);
		


	double inlier_ratio = inliers1.size() * 1.0 / matched1.size();
	cout << "A-KAZE Matching Results" << endl;
	cout << "*******************************" << endl;
	cout << "# Keypoints 1:                        \t" << kp1.size() << endl;
	cout << "# Keypoints 2:                        \t" << kp1_2.size() << endl;
	cout << "# Matches:                            \t" << matched1.size() << endl;
	cout << "# Inliers:                            \t" << inliers1.size() << endl;
	cout << "# Inliers Ratio:                      \t" << inlier_ratio << endl;
	cout << endl;

	
	//imshow("KeyPoints on Img1", out1);
	//imshow("KeyPoints on Img2", out2);


	imshow("full KPs on Img1", fullout1);

	imshow("full KPs on Img2", fullout2);

	//imwrite("res1.png", res);

	waitKey(0);
	return 0;

}

