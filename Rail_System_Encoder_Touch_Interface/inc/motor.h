/*
 * motor.h
 *
 *  Created on: Feb 25, 2016
 *      Author: smith
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include <stm32f7xx_hal.h>
#include <stdlib.h>
#include <system_setup.h>
#include <stm32746g_discovery.h>

class Motor
{
public:

	Motor(void);

	void InitMotor(void);
	void InitEncoder(void);

	void setEnable(bool enable);
	bool getEnable(void);
	void stop(void);
	void setMotorDirection(bool dir);
	bool getMotorDirection(void);
	void setDuty(int16_t inputDuty);
	int16_t getDuty(void);
	void setCount(int32_t count32);
	int32_t getCount(void);
	void setDirection(bool dir);
	bool getDirection(void);
	void setSpeed(float inputSpeed);
	float getSpeed(void);
	void setSpeedCommand(float inputSpeedCommand);
	float getSpeedCommand(void);
	void setSpeedError(float inputSpeedError);
	float getSpeedError(void);
	float getDistance(void);
	float getRevolution(void);
	void Error_Handler(void);

private:
	int16_t prescaler_;
	int16_t period_;
	bool enable_;
	bool dir_;
	int16_t duty_;
	uint16_t prev_counter_;
	int16_t overflows_;
	bool direction_;
	float speed_;
	float speedCommand_;
	float speedError_;
};
#endif /* MOTOR_H_ */
