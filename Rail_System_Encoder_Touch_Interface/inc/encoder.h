/*
 * encoder.h
 *
 *  Created on: Feb 29, 2016
 *      Author: smith
 */

#ifndef ENCODER_H_
#define ENCODER_H_

class Encoder
{
	public:
		Encoder(void);

		int32_t getCount(void);
		void setCount(int32_t count32);

		bool getDirection(void);
		void setDirection(bool inputDirection);

		float getRevolution(void);

		float getDistance(void);

		float getSpeed(void);
		void setSpeed(float inputSpeed);

		float getSpeedCommand(void);
		void setSpeedCommand(float inputSpeedCommand);

		float getSpeedError(void);
		void setSpeedError(float inputSpeedError);

		void Error_Handler(void);
	private:
		uint16_t prev_counter_;
		int16_t overflows_;
		float speed_;
		float speedCommand_;
		float speedError_;
		bool direction_;
};



#endif /* ENCODER_H_ */
