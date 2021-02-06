
#include "serialib.hpp"
#include "camera.hpp"
#include "pid.hpp"

#include <iostream>
#include <cmath>

#define BOUNCER         0
#define NORMAL          1
#define SQUARE          2
#define CROSS           3



//---------------------------------------------------//
#if defined (_WIN32) || defined(_WIN64)
#include <io.h>
#define SERIAL_PORT "COM4"
#endif
#ifdef __linux__
#include <unistd.h>
#define SERIAL_PORT "/dev/ttyUSB0"
#endif

#define CAMERA_ID  0      // CameraID

int mode = BOUNCER;
//---------------------------------------------------//


#define REFRESH_PERIOD  1      // In milliseconds
#define ESC_SC          27     // ESC scan code

#define X_BALANCE      230 //245
#define Y_BALANCE      175 //167

#define SQUARE_SIDE     85
#define SQUARE_PERIOD   75


using std::cout;
using std::cerr;
using std::endl;




int main(int argc, char* argv[])
{

	if (argc == 2)
	{
		if (!strcmp(argv[1], "bouncer"))
		{
			mode = BOUNCER;
		}
		if (!strcmp(argv[1], "normal"))
		{
			mode = NORMAL;
		}
		if (!strcmp(argv[1], "square"))
		{
			mode = SQUARE;
		}
		if (!strcmp(argv[1], "cross"))
		{
			mode = CROSS;
		}

	}

	// Serial object
	serialib serial;


	// Connection to serial port
	char errorOpening = serial.openDevice(SERIAL_PORT, 115200);

	// If connection fails, return the error code otherwise, display a success message
	if (errorOpening != 1) return errorOpening;
	cerr << "Successful connection to" << SERIAL_PORT << "\n" << endl;


	Camera camera(CAMERA_ID);
	if (camera.fail())
	{
		cerr << camera.getErrorStr() << endl;
		exit(EXIT_FAILURE);
	}

	Pid pidServoX("pidServoX_config");
	if (pidServoX.fail())
	{
		cerr << pidServoX.getErrorStr() << endl;
		exit(EXIT_FAILURE);
	}
	Pid pidServoY("pidServoY_config");
	if (pidServoY.fail())
	{
		cerr << pidServoY.getErrorStr() << endl;
		exit(EXIT_FAILURE);
	}

	switch (mode)
	{

	case NORMAL:
	{

		while (true)
		{
			camera.refresh();
			///
			cout << "-----------" << endl;
			cout << "X: " << camera.getX() << endl;
			cout << "Y: " << camera.getY() << endl;
			cout << "H: " << camera.getH() << endl;
			///

			pidServoX.compute(X_BALANCE - camera.getX());
			pidServoY.compute(Y_BALANCE - camera.getY());

			cout << "errorX: " << X_BALANCE - camera.getX() << endl;
			cout << "errorY: " << Y_BALANCE - camera.getY() << endl;

			cout << "signalX: " << pidServoX.getOutput() << endl;
			cout << "signalY: " << pidServoY.getOutput() << endl;

			
			std::string angle1_str = std::to_string(-pidServoY.getOutput());
			std::string angle2_str = std::to_string(pidServoY.getOutput());
			std::string angle3_str = std::to_string(pidServoX.getOutput());
			std::string angle4_str = std::to_string(-pidServoX.getOutput());

			//map function
			//(x - in_min)* (out_max - out_min) / (in_max - in_min) + out_min

			std::string x_str = std::to_string((camera.getX() - 0) * (200 - 0) / (490 - 0) + 0);
			std::string y_str = std::to_string((camera.getY() - 0) * (200 - 0) / (335 - 0) + 0);
			std::string h_str = std::to_string(camera.getH());
		

			std::string signal = angle1_str + ":" + angle2_str + ":" + angle3_str + ":" + angle4_str + ":" + x_str + ":" + y_str + ":" + h_str + "$";

		

			serial.writeString((char*)signal.c_str());


			cout << "-----------" << endl;


			if (cv::waitKey(REFRESH_PERIOD) == ESC_SC) // Waiting for 'ESC' key press for 30ms. If 'ESC' key is pressed, break loop
			{


				std::string signal = "0:0:0:0:0:0:0$";


				serial.writeString((char*)signal.c_str());

				cout << "'ESC' key was pressed." << endl;
				break;
			}
		}

		break;

	}

	case SQUARE:
	{

		int xSquare = X_BALANCE - SQUARE_SIDE;
		int ySquare = Y_BALANCE - SQUARE_SIDE;

		int count = 0;

		while (true)
		{
			camera.refresh();
			///
			cout << "-----------" << endl;
			cout << "X: " << camera.getX() << endl;
			cout << "Y: " << camera.getY() << endl;
			cout << "H: " << camera.getH() << endl;
			///

			count++;
			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 1)
			{
				xSquare = X_BALANCE + SQUARE_SIDE;
				ySquare = Y_BALANCE - SQUARE_SIDE;
			}

			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 2)
			{
				xSquare = X_BALANCE + SQUARE_SIDE;
				ySquare = Y_BALANCE + SQUARE_SIDE;
			}

			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 3)
			{
				xSquare = X_BALANCE - SQUARE_SIDE;
				ySquare = Y_BALANCE + SQUARE_SIDE;
			}

			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 4)
			{
				xSquare = X_BALANCE - SQUARE_SIDE;
				ySquare = Y_BALANCE - SQUARE_SIDE;

				count = 0;
			}

			pidServoX.compute(xSquare - camera.getX());
			pidServoY.compute(ySquare - camera.getY());

			cout << "squareX: " << xSquare << endl;
			cout << "squareY: " << ySquare << endl;

			cout << "errorX: " << xSquare - camera.getX() << endl;
			cout << "errorY: " << ySquare - camera.getY() << endl;

			cout << "signalX: " << pidServoX.getOutput() << endl;
			cout << "signalY: " << pidServoY.getOutput() << endl;

			std::string angle1_str = std::to_string(-pidServoY.getOutput());
			std::string angle2_str = std::to_string(pidServoY.getOutput());
			std::string angle3_str = std::to_string(pidServoX.getOutput());
			std::string angle4_str = std::to_string(-pidServoX.getOutput());

			//map function
			//(x - in_min)* (out_max - out_min) / (in_max - in_min) + out_min

			std::string x_str = std::to_string((camera.getX() - 0) * (200 - 0) / (490 - 0) + 0);
			std::string y_str = std::to_string((camera.getY() - 0) * (200 - 0) / (335 - 0) + 0);
			std::string h_str = std::to_string(camera.getH());


			std::string signal = angle1_str + ":" + angle2_str + ":" + angle3_str + ":" + angle4_str + ":" + x_str + ":" + y_str + ":" + h_str + "$";



			serial.writeString((char*)signal.c_str());


			cout << "-----------" << endl;


			if (cv::waitKey(REFRESH_PERIOD) == ESC_SC) // Waiting for 'ESC' key press for 30ms. If 'ESC' key is pressed, break loop
			{

				std::string signal = "0:0:0:0:0:0:0$";


				serial.writeString((char*)signal.c_str());

				cout << "'ESC' key was pressed." << endl;
				break;
			}
		}

		break;

	}

	case CROSS:
	{

		int xSquare = X_BALANCE - SQUARE_SIDE;
		int ySquare = Y_BALANCE - SQUARE_SIDE;

		int count = 0;

		while (true)
		{
			camera.refresh();
			///
			cout << "-----------" << endl;
			cout << "X: " << camera.getX() << endl;
			cout << "Y: " << camera.getY() << endl;
			cout << "H: " << camera.getH() << endl;
			///

			count++;
			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 1)
			{
				xSquare = X_BALANCE + SQUARE_SIDE;
				ySquare = Y_BALANCE - SQUARE_SIDE;
			}


			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 2)
			{
				xSquare = X_BALANCE - SQUARE_SIDE;
				ySquare = Y_BALANCE + SQUARE_SIDE;
			}


			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 3)
			{
				xSquare = X_BALANCE + SQUARE_SIDE;
				ySquare = Y_BALANCE + SQUARE_SIDE;
			}

			
			if (count == SQUARE_PERIOD / REFRESH_PERIOD * 4)
			{
				xSquare = X_BALANCE - SQUARE_SIDE;
				ySquare = Y_BALANCE - SQUARE_SIDE;

				count = 0;
			}

			pidServoX.compute(xSquare - camera.getX());
			pidServoY.compute(ySquare - camera.getY());

			cout << "squareX: " << xSquare << endl;
			cout << "squareY: " << ySquare << endl;

			cout << "errorX: " << xSquare - camera.getX() << endl;
			cout << "errorY: " << ySquare - camera.getY() << endl;

			cout << "signalX: " << pidServoX.getOutput() << endl;
			cout << "signalY: " << pidServoY.getOutput() << endl;

			std::string angle1_str = std::to_string(-pidServoY.getOutput());
			std::string angle2_str = std::to_string(pidServoY.getOutput());
			std::string angle3_str = std::to_string(pidServoX.getOutput());
			std::string angle4_str = std::to_string(-pidServoX.getOutput());

			//map function
			//(x - in_min)* (out_max - out_min) / (in_max - in_min) + out_min

			std::string x_str = std::to_string((camera.getX() - 0) * (200 - 0) / (490 - 0) + 0);
			std::string y_str = std::to_string((camera.getY() - 0) * (200 - 0) / (335 - 0) + 0);
			std::string h_str = std::to_string(camera.getH());


			std::string signal = angle1_str + ":" + angle2_str + ":" + angle3_str + ":" + angle4_str + ":" + x_str + ":" + y_str + ":" + h_str + "$";

			



			serial.writeString((char*)signal.c_str());


			cout << "-----------" << endl;


			if (cv::waitKey(REFRESH_PERIOD) == ESC_SC) // Waiting for 'ESC' key press for 30ms. If 'ESC' key is pressed, break loop
			{



				std::string signal = "0:0:0:0:0:0:0$";


				serial.writeString((char*)signal.c_str());

				cout << "'ESC' key was pressed." << endl;
				break;
			}
		}

		break;

	}

	case BOUNCER:
	{

		int count = 0;
		int bounceAngle = 0;
		while (true)
		{
			camera.refresh();
			///
			cout << "-----------" << endl;
			cout << "X: " << camera.getX() << endl;
			cout << "Y: " << camera.getY() << endl;
			cout << "H: " << camera.getH() << endl;
			///

			pidServoX.compute(X_BALANCE - camera.getX());
			pidServoY.compute(Y_BALANCE - camera.getY());

			cout << "errorX: " << X_BALANCE - camera.getX() << endl;
			cout << "errorY: " << Y_BALANCE - camera.getY() << endl;

			cout << "signalX: " << pidServoX.getOutput() << endl;
			cout << "signalY: " << pidServoY.getOutput() << endl;


			std::string angle1_str;
			std::string angle2_str;
			std::string angle3_str;
			std::string angle4_str;

			bounceAngle = 0;
						
			//(x-a)^2 + (y-b)^2 = R^2

			if ((pow((X_BALANCE - camera.getX()),2) +pow((Y_BALANCE - camera.getY()), 2)) < 3600)
			{
				bounceAngle = 3;
			}
			if ((pow((X_BALANCE - camera.getX()), 2) + pow((Y_BALANCE - camera.getY()), 2)) < 2025)
			{
				bounceAngle = 4;
			}
			if ((pow((X_BALANCE - camera.getX()), 2) + pow((Y_BALANCE - camera.getY()), 2)) < 900)
			{
				bounceAngle = 5;
			}

			if (camera.ballArea_ > camera.getObjectArea())
			{
				bounceAngle = 0;
			}

			count++;
			if (count % 2 == 0)
			{
				angle1_str = std::to_string((-pidServoY.getOutput()) + bounceAngle);
				angle2_str = std::to_string(pidServoY.getOutput() + bounceAngle);
				angle3_str = std::to_string(pidServoX.getOutput() + bounceAngle);
				angle4_str = std::to_string((-pidServoX.getOutput()) + bounceAngle);

			}
			else
			{
				angle1_str = std::to_string((-pidServoY.getOutput()) - bounceAngle);
				angle2_str = std::to_string(pidServoY.getOutput() - bounceAngle);
				angle3_str = std::to_string(pidServoX.getOutput() - bounceAngle);
				angle4_str = std::to_string((-pidServoX.getOutput()) - bounceAngle);
			}

			


			//map function
			//(x - in_min)* (out_max - out_min) / (in_max - in_min) + out_min

			std::string x_str = std::to_string((camera.getX() - 0) * (200 - 0) / (490 - 0) + 0);
			std::string y_str = std::to_string((camera.getY() - 0) * (200 - 0) / (335 - 0) + 0);
			std::string h_str = std::to_string(camera.getH());


			std::string signal = angle1_str + ":" + angle2_str + ":" + angle3_str + ":" + angle4_str + ":" + x_str + ":" + y_str + ":" + h_str + "$";

			



			serial.writeString((char*)signal.c_str());


			cout << "-----------" << endl;


			if (cv::waitKey(REFRESH_PERIOD) == ESC_SC) // Waiting for 'ESC' key press for 30ms. If 'ESC' key is pressed, break loop
			{

				std::string signal = "0:0:0:0:0:0:0$";

	
				serial.writeString((char*)signal.c_str());

				cout << "'ESC' key was pressed." << endl;
				break;
			}
		}

		break;

	}

	}


	exit(EXIT_SUCCESS);

}
