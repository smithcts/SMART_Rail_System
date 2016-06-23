/*
 * pwm.h
 *
 *  Created on: Feb 25, 2016
 *      Author: smith
 */

#ifndef MOTOR_H_
#define MOTOR_H_

class Motor
{
public:

	Motor(void);

	int32_t read(void);

	void setEnable(bool enable);

	bool getEnable(void);

	void stop(void);

	void setDuty(int16_t inputDuty);

	int16_t getDuty(void);

	void Error_Handler(void);

private:
	int16_t duty_;
	int16_t prescaler_;
	int16_t period_;
	bool enable_;
	TIM_HandleTypeDef TIM_HANDLE_;
};


#endif /* MOTOR_H_ */
