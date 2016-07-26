#ifndef PID_CONTROLLER_H_INCLUDED
#define PID_CONTROLLER_H_INCLUDED

class PidController{

public:

	PidController(float kp, float ki, float kd);

	float calculate(float error);

	void setPid(float kp, float ki, float kd);

private:
	float KP_;
	float KI_;
	float KD_;
};
#endif
