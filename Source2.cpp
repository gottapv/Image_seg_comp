#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <sstream>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

//Structs
struct seg_coords
{
	int minX, minY, maxX, maxY;
};

struct seg_Type{
	Mat segment;
	Point2d segment_coords;
//	bool found_match = false;
};


/// Global Variables
Mat load_img1, load_img2, img, templ, result;
const char* image_window = "Result Image after Detection";
vector<seg_Type> list_Segments_img1, list_Segments_img2; //List to have all the segments to be stored.
int match_method = 3;

//Change for the depth of search. //Range -1.0 to +1.0
double threshold_val = 0.90;

/// Function Headers
void MatchingMethod(int, void*);
void template_match();

//*************************
//Should try to set up a roi and the segment the images and process them according to their array cound, ROI helps in less processing.
//*************************

int main(int, char** argv)
{
	Mat load_img_to_crop1, load_img_to_crop2; 
	Mat frame_size;

	
	load_img_to_crop1 = imread("MinuteMaid-Top.png", 1);
	load_img_to_crop2 = imread("MinuteMaid-Top2-Align.png", 1);

	load_img_to_crop1 = load_img_to_crop1(Rect(300, 200, 900, 700));
	load_img_to_crop2 = load_img_to_crop2(Rect(300, 200, 900, 700));

	load_img1 = load_img_to_crop1; //imread("MinuteMaid-Top.png", 1);
	load_img2 = load_img_to_crop2; // imread("MinuteMaid-Top2-alignps.png", 1);

	if (load_img1.rows != load_img2.rows || load_img1.cols != load_img2.cols){
		cout << "Image sizes are not same" << endl;
		return -1;
	}
	else{
		frame_size.rows = load_img1.rows;
		frame_size.cols = load_img2.cols;
	}

	seg_Type temp_seg_type_for_image1, temp_seg_type_for_image2;
	//Mat sub_block_temp1, sub_block_temp2;
	int i_, j_;

	//number should have the same amount removed or added to each of the quality, /except for the roi for image 1 matcher/
	for (int i = 50; i < frame_size.cols - 60; i += 50){ //size -70, coz it will be -20 + -50 with the i
		for (int j = 50; j < frame_size.rows - 60; j += 30){
			
			i_ = i; j_ = j;

			temp_seg_type_for_image1.segment = load_img1(Rect(i_ - 5, j_ - 5, 50, 50)); //making checkboxes, since used -20, add 20+20 to height n width
			temp_seg_type_for_image1.segment_coords.x = i; temp_seg_type_for_image1.segment_coords.y = j;
			list_Segments_img1.push_back(temp_seg_type_for_image1);

			temp_seg_type_for_image2.segment = load_img2(Rect(i, j, 40, 40));
			temp_seg_type_for_image2.segment_coords.x = i; temp_seg_type_for_image2.segment_coords.y = j;
			list_Segments_img2.push_back(temp_seg_type_for_image2);

		}
	}

	stringstream ss1,ss2;

	string name = "segment_";
	string type = ".png";

	string folderName1 = "Image1_Segments";
	string folderName2 = "Image2_Segments";
	//string tempfolderName = "temp";
	//string tempcreatecommand = "mkdir " + tempfolderName;
	//system(tempcreatecommand.c_str());

	string folderCreateCommand1 = "mkdir " + folderName1;
	string folderCreateCommand2 = "mkdir " + folderName2;

	system(folderCreateCommand1.c_str());
	system(folderCreateCommand2.c_str());

	for (int i = 0; i < list_Segments_img1.size(); i++){
		
		ss1 << folderName1 << "/" << name << (i) << type;
		ss2 << folderName2 << "/" << name << (i) << type;

		//ss << name << (i) << type;
		string filename_with_path1 = ss1.str();
		ss1.str("");
		string filename_with_path2 = ss2.str();
		ss2.str("");


		imwrite(filename_with_path1, list_Segments_img1.at(i).segment);
		imwrite(filename_with_path2, list_Segments_img2.at(i).segment);

	}

	template_match();

	imshow(image_window, load_img2);
//	imwrite("Variation4.png", load_img2);

	waitKey(0);
	return 0;
}


void template_match(){
	/// Load image and template

	stringstream ss1;
	stringstream ss2;

	string name = "segment_";
	string type = ".png";

	string folderName1 = "C:/Insigma_OpenCV/Test_Project/Image1_Segments";
	string folderName2 = "C:/Insigma_OpenCV/Test_Project/Image2_Segments";
	
	for (int i = 0; i < list_Segments_img1.size(); i++){
		ss1 << folderName1 << "/" << name << (i) << type;
		ss2 << folderName2 << "/" << name << (i) << type;

		string filename_with_path1 = ss1.str();
		string filename_with_path2 = ss2.str();

		ss1.str("");
		ss2.str("");
	
		//	imwrite(filename_with_path1, list_Segments_img1.at(i).segment);
		img = imread(filename_with_path1, 1); 
		templ = imread(filename_with_path2, 1); 

		MatchingMethod(i, 0);

	}

//	img = imread("C:/Insigma_OpenCV/Test_Project/Image1_Segments/segment_52.png", 1); //("segment_36_big_seg_skew.png", 1); //
//	templ = imread("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_52.png", 1); //("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_36.png", 1);  //
	
	//img = imread("C:/Insigma_OpenCV/Test_Project/Image1_Segments/segment_36.png", 1);
	//templ = imread("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_36.png", 1);



	/*
	Mat i1 = imread("segment_36_big_seg_skew.png");
	Mat i2 = imread("segment_36_seg_skew.png");
	Mat i3 = imread("C:/Insigma_OpenCV/Test_Project/Image1_Segments/segment_36.png");
	Mat i4 = imread("C:/Insigma_OpenCV/Test_Project/Image2_Segments/segment_36.png");


	imshow("To be compared to Segment", i3);
	imshow("To be compared to Segment Skewed", i2);
	imshow("Segment small", i4);
	imshow("Segment small skewed", i1);*/



//	imshow("Image", img);
//	imshow("Template", templ);

}




void MatchingMethod(int templ_count, void*)
{
	/// Source image to display
	//Mat img_display;
	//img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols- templ.cols + 1;
	int result_rows = img.rows-templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
//	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
//	cv::threshold(result, result, 0.8, 1., CV_THRESH_TOZERO);

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	matchLoc = maxLoc;

	/// Show me what you got
	Point templ_temp_coords = list_Segments_img2.at(templ_count).segment_coords;

	if (maxVal >= threshold_val){
		//Simply Ingnore the matched areas
		//cout << "Template matches with the given Image at the marked area" << endl;


		//the below statement can be uncommented to get perfect output of the data
	//	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0,255,0), 2, 8, 0);
		
		
	
	}
	else{
		//should input Complate Image 2 and use its template Co-ordinates to mark the area
		//cout << "Template does not match with the given Image in the region it is taken from" << endl;
		
		//IMportant line below
	//	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0,0,255), 2, 8, 0);

		rectangle(load_img2, templ_temp_coords, Point(templ_temp_coords.x + templ.cols, templ_temp_coords.y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);
	}

//	cout << temp_count <<endl;
//	namedWindow(image_window, WINDOW_AUTOSIZE);
//	imshow(image_window, img_display);

	return;
}
