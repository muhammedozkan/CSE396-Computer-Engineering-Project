#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using std::cout;
using std::endl;

class Camera
{

public:
	Camera(int id);
	~Camera();

	void refresh();

	int getX();
	int getY();
	double getH();
	double getObjectArea();


	bool fail();
	std::string getErrorStr();

	// Constants for orange ball
	int lowH_ = 0;  // Hue (0 - 179)
	int highH_ = 55; 

	int lowS_ = 80;   // Saturation (0 - 255)145
	int highS_ = 255;   

	int lowV_ = 80;   // Value (0 - 255)145
	int highV_ = 255;   

	static const int ballArea_ = 20000;  // Approximate ball area (compare it with objArea_)
	static const int ballRadius_ = 40;      // Approximate ball radis to circle it with square

    const double maxH_ = 40.0;
	static const int maxArea_ = 1500000;
	const double pi_ = 3.14159265358979;

	cv::VideoCapture camera_;       // Web camera

private:
	void setFail();
	void setErrorStr(std::string errorString);
	void overlayImage(const cv::Mat& background, const cv::Mat& foreground, cv::Mat& output, cv::Point2i location);

	int x_;                         // If coordinates' values equal to (-1), it means
	int y_;                         // that there is no object on the frame
	double h_;                         // height object(cm) 


	cv::Mat imgOriginal_;           // New frame from camera
	cv::Mat imgHSV_;                // Original, converted to HSV
	cv::Mat imgThresholded_;        // Thresholded HSV image

	cv::Mat imgSquare_;             // For circling the ball in a square

	cv::Moments objMoments_;        // Moments of thresholded picture
	double  objMoment01_;           // Responsible for Y
	double  objMoment10_;           // Responsible for X
	double  objArea_;               // Number of pixels of the object

	double r1_;
	double r2_;

	double inverse_rate_;

	bool fail_;                     // True on fail
	std::string errorString_;       // If fail_ is true, here is a valid reason of the error
};

#endif // CAMERA_HPP