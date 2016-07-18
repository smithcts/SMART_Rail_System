#include "pid.h"
#include "arm_math.h"

PID::PID(void)
{
	arm_pid_instance_f32 PID;
	PID.Kp = 0;
	PID.Ki = 0;
	PID.Kd = 0;
	arm_pid_init_f32(&PID,1);
}

float getPid(float error)
{
	arm_pid_instance_f32 PID;
	return (arm_pid_f32(&PID, error));
}

void setPid(float kp, float ki, float kd)
{
	arm_pid_instance_f32 PID;
/*	kp_ = kp;
	ki_ = ki;
	kd_ = kd;
	PID.Kp = KP;
	PID.Ki = KI;
	PID.Kd = KD;*/
}

