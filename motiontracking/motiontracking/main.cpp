//motionTracking.cpp


#import "MAIL\MAIL\bin\Debug\MAIL.tlb"

#include "opencv2\imgproc\imgproc.hpp"
#include<cstdio>
#include"opencv2/highgui/highgui.hpp"
#include"opencv\cv.h"
#include<iostream>
#include <opencv2\core\core.hpp>

#include <opencv2/features2d/features2d.hpp>
#include "opencv2/imgcodecs.hpp"
#include<thread>
#include<chrono>
using namespace cv;
using namespace std;
using namespace MAIL;

//our sensitivity value to be used in the threshold() function
const static int SENSITIVITY_VALUE = 40;
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
const static int BLUR_SIZE =30;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = { 0,0 };
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0, 0, 0, 0);
int xcor, ycor;
int xobject, yobject;
//int to string helper function
string intToString(int number) {

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void searchForMovement(Mat thresholdImage, Mat &cameraFeed) {

	bool objectDetected = false;
	Mat temp;
	thresholdImage.copyTo(temp);
	//these two vectors needed for output of findContours
	vector<Mat>contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);// retrieves external contours

																					  //if contours vector is not empty, we have found some objects
	if (contours.size()>0)objectDetected = true;
	else objectDetected = false;

	if (objectDetected) {
		//the largest contour is found at the end of the contours vector
		//we will simply assume that the biggest contour is the object we are looking for.
		vector<Mat> largestContourVec;
		largestContourVec.push_back(contours.at(contours.size() - 1));
		//make a bounding rectangle around the largest contour then find its centroid
		//this will be the object's final estimated position.
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
		int ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;
		xobject = xpos;
		yobject = ypos;
		//update the objects positions by changing the 'theObject' array values
		theObject[0] = xpos, theObject[1] = ypos;
	}

	//make some temp x and y variables so we dont have to type out so much
	int x = theObject[0];
	int y = theObject[1];
	//draw some crosshairs on the object
	circle(cameraFeed, Point(x, y), 20, Scalar(0, 255, 0), 2);
	line(cameraFeed, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	line(cameraFeed, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	line(cameraFeed, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	line(cameraFeed, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	putText(cameraFeed, "Tracking object at (" + intToString(x) + "," + intToString(y) + ")", Point(x, y), 1, 1, Scalar(255, 0, 0), 2);



}
int main() {

	//some boolean variables for added functionality
	bool objectDetected = false;
	//these two can be toggled by pressing 'd' or 't'
	bool debugMode = false;
	bool trackingEnabled = true;
	//pause and resume code
	bool pause = false;
	//set up the matrices that we will need
	//the two frames we will be comparing
	Mat frame1, frame2,frame3,frame4;
	//their grayscale images (needed for absdiff() function)
	Mat grayImage1, grayImage2;
	//resulting difference image
	Mat differenceImage;
	//thresholded difference image (for use in findContours() function)
	Mat thresholdImage;
	//video capture object.
	VideoCapture capture;
	capture.open(0);
	int r=400;
	if (!capture.isOpened()) {
		cout << "ERROR ACQUIRING VIDEO FEED\n";
		getchar();
		return -1;
	}
	while (1) {

		
			
			//read first frame
			capture.read(frame1);
			//convert frame1 to gray scale for frame differencing
			cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
			//copy second frame
			capture.read(frame2);
			capture.read(frame3);
		
              //convert frame2 to gray scale for frame differencing
			cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
			//perform frame differencing with the sequential images. This will output an "intensity image"
			//do not confuse this with a threshold image, we will need to perform thresholding afterwards.
			absdiff(grayImage1, grayImage2, differenceImage);
			//threshold intensity image at a given sensitivity value
			threshold(differenceImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
			if (debugMode == true) {
				//show the difference image and threshold image
				imshow("Difference Image", differenceImage);
				imshow("Threshold Image", thresholdImage);
			}
			else {
				//if not in debug mode, destroy the windows so we don't see them anymore
				destroyWindow("Difference Image");
				destroyWindow("Threshold Image");
			}
			//use blur() to smooth the image, remove possible noise and
			//increase the size of the object we are trying to track. (Much like dilate and erode)
			blur(thresholdImage, thresholdImage, Size(BLUR_SIZE,BLUR_SIZE));
			//threshold again to obtain binary image from blur output
			threshold(thresholdImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);

			// trigger for motion 
			int sum=0;
		
			if (r >= 1000)
			{
			
				for (int j = 0; j < thresholdImage.rows; j++)
				{
					for (int i = 0; i < thresholdImage.cols; i++)
					{
						sum = sum + thresholdImage.at<uchar>(j, i);
						if (sum > 0)
						{

							break;
						}
					}
					if (sum > 0)
					{
						break;
					}
				}

				if (sum > 0)
				{
				
					// Initialize COM.
					
					
					imwrite("motiontracking\\motiontracking\\captured.jpg", frame4);
			
					r = 0;
					pause = true;
					HRESULT hr = CoInitialize(NULL);

					// Create the interface pointer.
					ICalculatorPtr pICalc(__uuidof(Class1));

					long lResult = 0;

					// Call the Add method.
					pICalc->mailsms();

					//wprintf(L"The result is %d\n", lResult);


					// Uninitialize COM.
					CoUninitialize();

					pause = false;


				}

				
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			++r;

			if (debugMode == true) {
				//show the threshold image after it's been "blurred"
				imshow("Final", thresholdImage);
				cout << sum << "\n";
				cout << "r is " << r << "\n";
			}
			else {
				destroyWindow("Final");
				//if not in debug mode, destroy the windows so we don't see them anymore

			}
			
			//if tracking enabled, search for contours in our thresholded image
			if (trackingEnabled)
			{
				searchForMovement(thresholdImage, frame1);
			}
			//show our captured frame
			imshow("Frame1", frame1);
			//check to see if a button has been pressed.
			//this 10ms delay is necessary for proper operation of this program
			//if removed, frames will not have enough time to referesh and a blank 
			//image will appear.
			switch (waitKey(10)) {

			case 27: //'esc' key has been pressed, exit program.
				return 0;
			case 116: //'t' has been pressed. this will toggle tracking
				trackingEnabled = !trackingEnabled;
				if (trackingEnabled == false) cout << "Tracking disabled." << endl;
				else cout << "Tracking enabled." << endl;
				break;
			case 100: //'d' has been pressed. this will debug mode
				debugMode = !debugMode;
				if (debugMode == false) cout << "Debug mode disabled." << endl;
				else cout << "Debug mode enabled." << endl;
				break;
			case 112: //'p' has been pressed. this will pause/resume the code.
				pause = !pause;
				if (pause == true) {
					cout << "Code paused, press 'p' again to resume" << endl;
					while (pause == true) {
						//stay in this loop until 
						switch (waitKey()) {
							//a switch statement inside a switch statement? Mind blown.
						case 112:
							//change pause back to false
							pause = false;
							cout << "Code resumed." << endl;
							break;
						}
					}
				}


			}


	}

	return 0;

}