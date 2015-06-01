#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//Loading the Image 1
	string filename = "MinuteMaid-Top.png"; //File name, as in the file from directory
	Mat image;								//Matrix for image
	//We load the image like a matrix
	// Read the file
	image = imread(filename.c_str(), IMREAD_COLOR);
	// Check if data is invalid
	if (!image.data) {
		cout << "ERROR: Could not open or find the image " << filename << std::endl;
		return -1;
	}
	
	//Loading the Image 2
	string filename2 = "MinuteMaid-Top2-Align.png"; //Giving the file name, given in the project file directory
	Mat image2;
	// Read the file
	image2 = imread(filename2.c_str(), IMREAD_COLOR);
	// Check if data is invalid
	if (!image.data) {
		cout << "ERROR: Could not open or find the image " << filename2 << std::endl;
		return -1;
	}

	//Declaring Variables
	cv::Mat bg_frame;	//Background frame
	bg_frame = image;	//Copying image1 into the frame
	cv::Mat new_frame;	//New Frame, to compare with Background frame
	new_frame = image2;	//Copying Image2 for comparing. (Loading Image into the frame)
	cv::Mat frame_diff;	//Matrix for loading the frame differences

	cv::absdiff(bg_frame, new_frame, frame_diff); //function to calculate the differences of images

	//For some reason the threshold values are not being calculated using inbuilt functions(lines below).
	
	//cv::threshold(motion, motion, 80, 255, cv::THRESH_BINARY);
	//cv::erode(motion, motion, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

	cv::Mat foregroundMask = cv::Mat::zeros(frame_diff.rows, frame_diff.cols, CV_8UC1);

	float threshold = 65.0f;
	float dist;
	
	//Function for calc the threshold
	for (int j = 0; j < frame_diff.rows; ++j){
		for (int i = 0; i < frame_diff.cols; ++i)
		{
			cv::Vec3b pix = frame_diff.at<cv::Vec3b>(j, i);

			dist = (pix[0] * pix[0] + pix[1] * pix[1] + pix[2] * pix[2]);
			dist = sqrt(dist);

			if (dist > threshold)
			{
				foregroundMask.at<unsigned char>(j, i) = 255;
			}
		}
	}

	//for more detail and clarity over the image.
	//cv::cvtColor(backgroundImage, HSVbackgroundImagebg, CV_BGR2HSV); cv::cvtColor(currentImage, HSV_currentImage, CV_BGR2HSV);


	/*
	int height = image.rows;
	int width = image.cols;

	printf("Processing a %dx%d image\n", height, width);

	cv::Size smallSize(110, 70);

	std::vector < Mat > smallImages;
	namedWindow("smallImages ", CV_WINDOW_AUTOSIZE);

	for (int y = 0; y < image.rows; y += smallSize.height)
	{
		for (int x = 0; x < image.cols; x += smallSize.width)
		{
			cv::Rect rect = cv::Rect(x, y, smallSize.width, smallSize.height);
			smallImages.push_back(cv::Mat(image, rect));
			imshow("smallImages", cv::Mat(image, rect));
			waitKey(0);
		}
	}

	*/

	//	cv::Mat diffImageOut;
	//	diffImageOut = image2 - foregroundMask;
	//	cv::absdiff(cam_frame,foregroundMask,diffImageOut);

	//Displaying the image (diff Image)
	
	string windowName1 = "Difference in frames";
	namedWindow(windowName1, WINDOW_AUTOSIZE);
	//Show our image inside it.
	imshow("Difference in frames", frame_diff);
	
	//Mat segment_sample = foregroundMask(Rect(150, 50, 150, 250));
	Mat rect_segment1 = foregroundMask(Rect(425, 400, 750, 200));
	imshow("Threshold segment 1", rect_segment1);
	Mat image_segment1 = frame_diff(Rect(425, 400, 750, 200));
	imshow("Image Segment 1", image_segment1);

	rect_segment1;

	for (int i = 0; i < rect_segment1.cols; i += 50){
		cout << "i val = " << i << endl;
	}

	//displaying the resultant images (Thershold)
	string windowName = "Threshold of Image differences";
	namedWindow(windowName, WINDOW_AUTOSIZE);
	// Show our image inside it.
	imshow("Threshold of Image differences", foregroundMask);

	waitKey(0);                                          // Wait for a keystroke in the window
	
	return 0;

}