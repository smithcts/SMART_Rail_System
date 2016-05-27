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

	void start(void);

	void stop(void);

	void dutyCycle(int16_t duty);

	void Error_Handler(void);

private:
	int16_t duty_;
	TIM_HandleTypeDef TIM_HANDLE_;
};


#endif /* MOTOR_H_ */
