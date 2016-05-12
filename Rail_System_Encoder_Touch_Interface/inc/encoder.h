/*
 * encoder.h
 *
 *  Created on: Feb 29, 2016
 *      Author: smith
 */

#ifndef ENCODER_H_
#define ENCODER_H_



typedef enum
{
	Azimuthal_Encoder,
	Claw_Encoder,
	Vertical_Encoder
}encoder_id_t;

class Encoder
{
	public:
		Encoder(encoder_id_t id);

		int32_t read(void);

		int16_t direction(void);

		void Error_Handler(void);

	private:
		encoder_id_t encoder_id_;
		uint16_t prev_counter_;
		int16_t overflows_;

};



#endif /* ENCODER_H_ */
