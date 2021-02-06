#include "camera.hpp"

// Constructor
Camera::Camera(int id) : x_(0),
y_(0),

camera_(id, cv::CAP_DSHOW),

imgOriginal_(),
imgHSV_(),
imgThresholded_(),

objMoments_(),
objMoment01_(0.0),
objMoment10_(0.0),
objArea_(0.0),

fail_(false),
errorString_("")
{
	camera_.open(id);
	if (!camera_.isOpened())  // If not success, exit program
	{
		this->setFail();
		this->setErrorStr("Failed to open camera.");
	}

	

	camera_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	camera_.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
	
	
	camera_.set(cv::CAP_PROP_FPS, 330);
	
	cv::Mat imgTmp;
	camera_.read(imgTmp);

	r1_ = sqrt(maxArea_ / pi_);//find r1 and r2
	r2_ = sqrt(ballArea_ / pi_);

	inverse_rate_ = (r1_ - r2_) / maxH_;//calculate ratio
	
	//crop image
	imgTmp = imgTmp(cv::Rect(95, 5, 490, 335));

	// Creating a black image with the size as the camera output
	imgSquare_ = cv::Mat::zeros(imgTmp.size(), CV_8UC3);
}

// Destructor
Camera::~Camera()
{
	x_ = -1;
	y_ = -1;

	camera_.~VideoCapture();

	imgOriginal_.~Mat();
	imgHSV_.~Mat();
	imgThresholded_.~Mat();

	objMoments_.~Moments();
	objMoment01_ = -1;
	objMoment10_ = -1;
	objArea_ = -1;

	fail_ = true;
	errorString_ = "";
}

// Methods

void
Camera::refresh()
{
	int ret = 0;

	ret = camera_.read(imgOriginal_); // Reading new frame from video stream
	cv::flip(imgOriginal_, imgOriginal_, 0);
	

	//crop image
	imgOriginal_ = imgOriginal_(cv::Rect(95, 5, 490, 335));
	

	if (!ret)
	{
		this->setFail();
		this->setErrorStr("Failed to read frame from video stream.");

		return;
	}

	cv::cvtColor(imgOriginal_, imgHSV_, cv::COLOR_BGR2HSV); // Converting the captured frame from BGR to HSV

	cv::inRange(imgHSV_,                                     // Thresholding the image
		cv::Scalar(lowH_, lowS_, lowV_),
		cv::Scalar(highH_, highS_, highV_),
		imgThresholded_);

	// Morphological opening (removes small objects from the foreground)
	cv::erode(imgThresholded_, imgThresholded_, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::dilate(imgThresholded_, imgThresholded_, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));


	// Morphological closing (removes small holes from the foreground)
	cv::dilate(imgThresholded_, imgThresholded_, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::erode(imgThresholded_, imgThresholded_, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Calculating the moments of the thresholded image
	objMoments_ = cv::moments(imgThresholded_);

	objMoment01_ = objMoments_.m01;
	objMoment10_ = objMoments_.m10;
	objArea_ = objMoments_.m00;
	
	if (objArea_ > ballArea_)
	{
		x_ = objMoment10_ / objArea_ ;
		y_ = objMoment01_ / objArea_ ;
		
		// Drawing red square around the ball
		line(imgSquare_, cv::Point(x_ - ballRadius_, y_ + ballRadius_),        // Vertical lines
			cv::Point(x_ - ballRadius_, y_ - ballRadius_),        //
			cv::Scalar(255, 0, 255), 2.5);                              //
																  //
		line(imgSquare_, cv::Point(x_ + ballRadius_, y_ + ballRadius_),        //
			cv::Point(x_ + ballRadius_, y_ - ballRadius_),        //
			cv::Scalar(255, 0, 255), 2.5);                              //

		line(imgSquare_, cv::Point(x_ - ballRadius_, y_ + ballRadius_),        // Horizontal lines
			cv::Point(x_ + ballRadius_, y_ + ballRadius_),        //
			cv::Scalar(255, 0, 255), 2.5);                              //
																  //
		line(imgSquare_, cv::Point(x_ - ballRadius_, y_ - ballRadius_),        //
			cv::Point(x_ + ballRadius_, y_ - ballRadius_),        //
			cv::Scalar(255, 0, 255), 2.5);                              //
	}

	cv::imshow("Thresholded Image", imgThresholded_); // Showing the thresholded image

	imgOriginal_ = imgOriginal_ + imgSquare_;
	cv::imshow("Original", imgOriginal_); // Showing the original image



	/* Create trackbars in "threshImg" window to adjust according to object and environment.*/
	cv::createTrackbar("LowH", "Thresholded Image", &lowH_, 255);
	cv::createTrackbar("HighH", "Thresholded Image", &highH_, 255);

	cv::createTrackbar("LowS", "Thresholded Image", &lowS_, 255);
	cv::createTrackbar("HighS", "Thresholded Image", &highS_, 255);

	cv::createTrackbar("LowV", "Thresholded Image", &lowV_, 255);
	cv::createTrackbar("HighV", "Thresholded Image", &highV_, 255);


	
	imgSquare_ = cv::Mat::zeros(imgSquare_.size(), CV_8UC3); // Clearing the imgSquare_

	
	h_ = -((sqrt(objArea_ / pi_) / inverse_rate_) - maxH_);

	if (h_ < 0)
	{
		h_ = 0;
	}


   cv::Mat background = cv::imread("table.png", cv::IMREAD_UNCHANGED);

   cv::Mat ball = cv::imread("ball.png", cv::IMREAD_UNCHANGED);


   // classical map equation 
   //(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

   int size = (h_ - 0) * (140 - 60) / (40 - 0) + 60;
   cv::resize(ball, ball, cv::Size(size, size));

   cv::Mat result;

   overlayImage(background, ball, result, cv::Point(x_- (size/2), y_- (size / 2)));

   std::string text = "X: " + std::to_string(x_) + " Y: " + std::to_string(y_) + " H: " + std::to_string(h_);
   cv::putText(result, text, cv::Point(1, 390), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 150, 150), 1);

   cv::imshow("2D Animation", result);


}

void Camera::overlayImage(const cv::Mat& background, const cv::Mat& foreground,	cv::Mat& output, cv::Point2i location)
{
	background.copyTo(output);


	// start at the row indicated by location, or at row 0 if location.y is negative.
	for (int y = std::max(location.y, 0); y < background.rows; ++y)
	{
		int fY = y - location.y; // because of the translation

		// we are done of we have processed all rows of the foreground image.
		if (fY >= foreground.rows)
			break;

		// start at the column indicated by location, 

		// or at column 0 if location.x is negative.
		for (int x = std::max(location.x, 0); x < background.cols; ++x)
		{
			int fX = x - location.x; // because of the translation.

			// we are done with this row if the column is outside of the foreground image.
			if (fX >= foreground.cols)
				break;

			// determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
			double opacity =
				((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])/ 255.;


			// and now combine the background and foreground pixel, using the opacity, 

			// but only if opacity > 0.
			for (int c = 0; opacity > 0 && c < output.channels(); ++c)
			{
				unsigned char foregroundPx =
					foreground.data[fY * foreground.step + fX * foreground.channels() + c];
				unsigned char backgroundPx =background.data[y * background.step + x * background.channels() + c];
				output.data[y * output.step + output.channels() * x + c] =backgroundPx * (1. - opacity) + foregroundPx * opacity;
			}
		}
	}
}



int
Camera::getX()
{
	return x_;
}

double
Camera::getH()
{
	return h_;
}

double
Camera::getObjectArea()
{
	return objArea_;
}

int
Camera::getY()
{
	return y_;
}

void
Camera::setFail()
{
	fail_ = true;
}

bool
Camera::fail()
{
	return fail_;
}

void
Camera::setErrorStr(std::string errorString)
{
	errorString_ = errorString;
}

std::string
Camera::getErrorStr()
{
	return errorString_;
}