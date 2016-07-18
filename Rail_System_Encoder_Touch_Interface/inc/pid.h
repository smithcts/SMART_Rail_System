#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED

class PID{

public:

	PID(void);

	void setPid(float kp, float ki, float kd);

	float getPid(float error);

private:
	float KP;
	float KI;
	float KD;
};
#endif
