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

	PID(void);
	float getIntegral(void);
	float getProportional(void);
	float calculate(float error, float kp, float ki, float dt);
private:
	float proportional_;
	float integral_;

};



#endif /* PID_CONTROL_H_ */
