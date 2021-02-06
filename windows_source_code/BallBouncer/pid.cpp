#include "pid.hpp"

#include <string>
#include <iostream>
#include <fstream>

// Constructors

Pid::Pid() : wP_(0.0),
wI_(0.0),
wD_(0.0),

lastInput_(0.0),
integralSum_(0.0),

output_(0.0),

period_(0.0),

minOutput_(0.0),
maxOutput_(0.0),

fail_(false),
errorString_("")
{
	// Everything is in initialization list
}

Pid::Pid(const char* configPath)
{
	initByFile(configPath);
}

// Destructor
Pid::~Pid()
{
	wP_ = -1;
	wI_ = -1;
	wD_ = -1;

	integralSum_ = -1;
	lastInput_ = -1;

	output_ = -1;

	period_ = -1;

	minOutput_ = -1;
	maxOutput_ = -1;

	fail_ = true;
	errorString_ = "";
}

// Methods

void
Pid::initByFile(const char* configPath)
{
	if (configPath == NULL)
	{
		this->setFail();
		this->setErrorStr("NULL pointer was passed to initByFile().");

		return;
	}

	std::ifstream configFile(configPath, std::ifstream::in);
	if (configFile.fail())
	{
		this->setFail();
		this->setErrorStr("Failed to open file (" + (std::string)configPath + ").");

		return;
	}

	std::string tmpStr;

	// PWeight
	configFile >> tmpStr;
	configFile >> wP_;

	// IWeight
	configFile >> tmpStr;
	configFile >> wI_;

	// DWeight
	configFile >> tmpStr;
	configFile >> wD_;

	// Period
	configFile >> tmpStr;
	configFile >> period_;

	// MaxOutput
	configFile >> tmpStr;
	configFile >> maxOutput_;

	// MinOutput
	configFile >> tmpStr;
	configFile >> minOutput_;

	configFile.close();

	// Initializing other fields
	integralSum_ = 0;
	lastInput_ = 0;

	output_ = 0;

	fail_ = false;
	errorString_ = "";
}

void
Pid::compute(double input)
{
	double proportional = wP_ * input;
	double integral = integralSum_ + wI_ * input * period_;
	double derivative = wD_ * (input - lastInput_) / period_;

	integralSum_ = integral;
	lastInput_ = input;

	double result = proportional + integral + derivative;

	output_ = result;
	if (result > maxOutput_)
		output_ = maxOutput_;
	if (result < minOutput_)
		output_ = minOutput_;



}

double
Pid::getOutput()
{
	return output_;
}

void
Pid::setFail()
{
	fail_ = true;
}

bool
Pid::fail()
{
	return fail_;
}

void
Pid::setErrorStr(std::string errorString)
{
	errorString_ = errorString;
}

std::string
Pid::getErrorStr()
{
	return errorString_;
}
