/*
 * pid_control.h
 *
 *  Created on: Aug 10, 2016
 *      Author: smith
 */

#ifndef PID_CONTROL_H_
#define PID_CONTROL_H_

class PID
{
public:

	PID(float kp, float ki, float kd);
	void setGain(float kp, float ki, float kd);
	float getIntegral(void);
	void resetIntegral(void);
	float getProportional(void);
	float calculate(float error, float dt);
private:
	float kp_;
	float ki_;
	float kd_;
	float proportional_;
	float integral_;
	float derivative_;

};



#endif /* PID_CONTROL_H_ */
