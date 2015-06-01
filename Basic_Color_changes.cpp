#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstring>

using namespace cv;
using namespace std;


void copyToDestinationImage(Mat &image, int columnOffset, Mat &dstImage) {

	int i, j;

	for (j = 0; j < image.cols; j++) {
		for (i = 0; i < image.rows; i++) {
			Vec3b  pixel = image.at<Vec3b>(i, j);

			dstImage.at<Vec3b>(i, j + columnOffset) = pixel;
		}
	}
}

void performFunction(Mat &image, int functionIndex) {
	int i, j;

	for (j = 0; j < image.cols; j++) {
		for (i = 0; i < image.rows; i++) {
			Vec3b  pixel = image.at<Vec3b>(i, j);

			uchar blue = pixel.val[0];
			uchar green = pixel.val[1];
			uchar red = pixel.val[2];

			if (pixel.val[functionIndex] > 10.0)
				pixel.val[functionIndex] = 255.00;
			else
				pixel.val[functionIndex] = 0.0;

			image.at<Vec3b>(i, j) = pixel;
		}
	}
}

void doTask(int functionIndex, Mat &origImage, Mat &newImage, Mat &dstImage) {

	// Copy original image
	origImage.copyTo(newImage);

	// Perform function
	performFunction(newImage, functionIndex);

	// Copy source image into destination image 
	copyToDestinationImage(newImage, origImage.cols, dstImage);
}

int main(int argc, char **argv) {

	string filename = "MinuteMaid-Top.png";
	Mat image;

	// Read the file
	image = imread(filename.c_str(), IMREAD_COLOR);

	// Check if data is invalid
	if (!image.data) {
		cout << "ERROR: Could not open or find the image " << filename << std::endl;
		return -1;
	}

	// Clone the image
	Mat modImage = image.clone();

	// Make window image
	Mat dstImage;
	dstImage.create(image.rows, image.cols *2 , image.type());

	// Copy source image into destination image (left side)
	copyToDestinationImage(image, 0, dstImage);

	// Copy original image
	image.copyTo(modImage);

	// Copy source image into destination image (right side)
	copyToDestinationImage(modImage, image.cols, dstImage);

	// Create a window for display
	string windowName = "Display window";
	namedWindow(windowName, WINDOW_AUTOSIZE);

	// Show our image inside it.
	imshow("Display window", dstImage);

	// Wait for a keystroke in the window
	uchar key;
	while ((key = waitKey(30)) != 27) {

		//cout << "KEY: " << key << endl;

		switch (key) {
		case '1':
			// Red threshold
			doTask(2, image, modImage, dstImage);
			break;
		case '2':
			// Green threshold
			doTask(1, image, modImage, dstImage);
			break;
		case '3':
			// Blue threshold
			doTask(0, image, modImage, dstImage);
			break;
		default:
			break;
		}

		// Show our image inside it.
		imshow("Display window", dstImage);
	}

	// Cleanup window
	destroyWindow(windowName);

	// Cleanup image
	image.release();
	dstImage.release();

	return 0;
}
