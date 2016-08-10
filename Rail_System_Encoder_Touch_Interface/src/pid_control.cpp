/*
 * pid_control.cpp
 *
 *  Created on: Aug 10, 2016
 *      Author: smith
 */

#include "pid_control.h"

PID::PID(void):
	proportional_ (0),
	integral_ (0)
		{

		}
float PID::getIntegral(void)
{
	return integral_;
}
float PID::getProportional(void)
{
	return proportional_;
}
float PID::calculate(float error, float kp, float ki, float dt)
{
	proportional_ = kp * error;
	integral_ += ki * error * dt;

	if (integral_ > 40.0f) integral_ = 40.0f;
	if (integral_ < -40.0f) integral_ = -40.0f;

	return(proportional_ + integral_);
}


