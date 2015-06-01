#include <stdio.h>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <sstream>

using namespace cv;
using namespace std;

Mat load_img1, load_img2, load_template, res;

struct seg_coords
{
	int minX, minY, maxX, maxY;
};

struct seg_Type{
	Mat segment;
	Point2d segment_coords;
	bool found_match = false;
};

int main(int argc, char** argv)
{

	load_img1 = imread("MinuteMaid-Top.png");
	load_img2 = imread("MinuteMaid-Top2-Align.png");

	//Manually cropped a block of Image from Load_img1("MinuteMaid-Top") and loading as an template
	Mat load_template = imread("MinuteMaid-Top-Template.png");

	vector<seg_Type> list_Segments_img1, list_Segments_img2; //List to have all the segments to be stored.

	if (load_img1.rows != load_img2.rows || load_img1.cols != load_img2.cols){
		cout << "Image sizes are not same" <<endl;
		return -1;
	}

	seg_Type temp_seg_type;
	Mat sub_block_temp1, sub_block_temp2;
	int i_, j_;
	for (int i = 50; i < load_img1.cols-120; i += 100){
		for (int j = 50; j < load_img1.rows-120; j += 100){
			i_ = i; j_ = j;
			temp_seg_type.segment = load_img1(Rect(i_-20, j_-20, 140, 140));
			temp_seg_type.segment_coords.x = i; temp_seg_type.segment_coords.y = j;
			list_Segments_img1.push_back(temp_seg_type);
		}
	}

	for (int i = 50; i < load_img2.cols - 120; i += 100){
		for (int j = 50; j < load_img2.rows - 120; j += 100){
			temp_seg_type.segment = load_img2(Rect(i, j, 100, 100));
			temp_seg_type.segment_coords.x = i; temp_seg_type.segment_coords.y = j;
			list_Segments_img2.push_back(temp_seg_type);
		}
	}

	stringstream ss2;
	stringstream ss1;

	string name = "segment_";
	string type = ".png";

	string folderName1 = "Image1_Segments";
	string folderName2 = "Image2_Segments";
	string folderCreateCommand1 = "mkdir " + folderName1;
	string folderCreateCommand2 = "mkdir " + folderName2;

	system(folderCreateCommand1.c_str());
	system(folderCreateCommand2.c_str());

	for (int i = 0; i < list_Segments_img1.size(); i++){
		ss1 << folderName1 << "/" << name << (i) << type;
		//ss << name << (i) << type;
		string filename_with_path1 = ss1.str();
		ss1.str("");
		imwrite(filename_with_path1, list_Segments_img1.at(i).segment);
	}

	for (int i = 0; i < list_Segments_img2.size(); i++){
		ss2 << folderName2 << "/" << name << (i) << type;
		//ss << name << (i) << type;
		string filename_with_path2 = ss2.str();
		ss2.str("");
		imwrite(filename_with_path2, list_Segments_img2.at(i).segment);
	}

	

	Mat template_temp_1 = list_Segments_img1[50].segment;
	Mat template_temp_2 = list_Segments_img2[50].segment;
	//load_img_array.assign((float*)load_img1.datastart, (float*)load_img1.dataend);
	//std::vector<float> template_temp_array;
	//template_temp_array.assign((float*)template_temp.datastart, (float*)template_temp.dataend);
	Mat* load_temp_ex;
	Mat res;

	//res.create(res.rows, res.cols, CV_32FC1);
	//cvMatchTemplate(&load_img1_array, &template_temp1, &result, CV_TM_SQDIFF);
	
	//cvMatchTemplate(&load_img1, &load_template, &res, CV_TM_CCORR_NORMED);
	
	imshow("Segment51 of Image 1", list_Segments_img1.at(50).segment);
	cout << "x&y i1 at 23 -" << list_Segments_img2.at(23).segment_coords.x << "-" << list_Segments_img2.at(23).segment_coords.y << endl;
	cout << "x&y i1 at 24 -" << list_Segments_img2.at(24).segment_coords.x << "-" << list_Segments_img2.at(24).segment_coords.y << endl;
	cout << "x&y i1 at 30 -" << list_Segments_img2.at(30).segment_coords.x << "-" << list_Segments_img2.at(30).segment_coords.y << endl;
	cout << "x&y i1 at 31 -" << list_Segments_img2.at(31).segment_coords.x << "-" << list_Segments_img2.at(31).segment_coords.y << endl;

	imshow("Segment51 of Image 2", list_Segments_img2.at(50).segment);

	//imshow("Result", res);
	
	waitKey(0);

	return 0;
}