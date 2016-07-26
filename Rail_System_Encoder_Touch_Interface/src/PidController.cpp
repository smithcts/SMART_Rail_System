#include "main.h"
#include "PidController.h"

	arm_pid_instance_f32 PID;
PidController::PidController(float kp, float ki, float kd)
{
	KP_ = kp;
	KI_ = ki;
	KD_ = kd;

	PID.Kp = KP_;
	PID.Ki = KI_;
	PID.Kd = KD_;
}

float PidController::calculate(float error)
{
	arm_pid_init_f32(&PID,1);
	return arm_pid_f32(&PID, error);
}

void PidController::setPid(float kp, float ki, float kd)
{
	KP_ = kp;
	KI_ = ki;
	KD_ = kd;

	PID.Kp = KP_;
	PID.Ki = KI_;
	PID.Kd = KD_;
}

