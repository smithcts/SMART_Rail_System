/*
 * pid_control.cpp
 *
 *  Created on: Aug 10, 2016
 *      Author: smith
 */

#include "pid_control.h"

PID::PID(float kp, float ki, float kd):
	proportional_ (0),
	integral_ (0)
		{
		setGain(kp,ki,kd);
		}
void PID::setGain(float kp, float ki, float kd)
{
	kp_ = kp;
	ki_ = ki;
	kd_ = kd;
}
float PID::getIntegral(void)
{
	return integral_;
}
void PID::resetIntegral(void)
{
	integral_ = 0;
}
float PID::getProportional(void)
{
	return proportional_;
}
float PID::calculate(float error, float dt)
{
	float oldError = 0;
	proportional_ = kp_ * error;
	integral_ += ki_ * error * dt;
	derivative_ = kd_ * (error - oldError) * dt;

	if (integral_ > 40.0f) integral_ = 40.0f;
	if (integral_ < -40.0f) integral_ = -40.0f;

	oldError = error;

	return(proportional_ + integral_ + derivative_);
}


