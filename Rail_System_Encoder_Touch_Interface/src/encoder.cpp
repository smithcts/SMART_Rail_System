/*
 * encoder.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: smith
 */

#include "main.h"

	/* Timer handler declaration */
	TIM_HandleTypeDef    Encoder_Handle;
	/* Timer Encoder Configuration Structure declaration */
	TIM_Encoder_InitTypeDef sEncoderConfig;


Encoder::Encoder(void)
{
	speedCommand_ = 0;
	prev_counter_ = 0;
	overflows_ = 0;
	  /* -1- Initialize TIM1 to handle the encoder sensor */
	  /* Initialize TIM1 peripheral as follows:
	       + Period = 65535
	       + Prescaler = 0
	       + ClockDivision = 0
	       + Counter direction = Up
	  */
	  Encoder_Handle.Instance = TIM8;

	  Encoder_Handle.Init.Period            = 65535;
	  Encoder_Handle.Init.Prescaler         = 0;
	  Encoder_Handle.Init.ClockDivision     = 0;
	  Encoder_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	  Encoder_Handle.Init.RepetitionCounter = 0;

	  sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TI12;

	  sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING;
	  sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;
	  sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1;
	  sEncoderConfig.IC1Filter          = 0;

	  sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_RISING;
	  sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;
	  sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1;
	  sEncoderConfig.IC2Filter          = 0;

	  if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK)
	  {
	    /* Initialization Error */
	    Error_Handler();
	  }

	  /* Start the encoder interface */
	  HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

}

int32_t Encoder::getCount(void)
{
	    uint16_t counter; // 16 bit counter of timer
	    int32_t count32;  // 32 bit counter which accounts for timer overflows

	    counter =__HAL_TIM_GET_COUNTER(&Encoder_Handle);

	    // The following assumes this function is called frequently enough that
	    // the encoder cannot change more 0x8000 counts between calls, and that
	    // the counter overflows from 0xffff to 0 and underflows from 0 to 0xffff
	    if ((prev_counter_ > 0xc000) && (counter < 0x4000))
	    {
	        overflows_ += 1; // overflow
	    }

	    else if ((prev_counter_ < 0x4000) && (counter > 0xc000))
	    {
	        overflows_ -= 1; // underflow
	    }

	    count32 = overflows_*0x10000 + counter;
	    prev_counter_ = counter;

	    return count32;
}

bool Encoder::getDirection(void) {
	return (__HAL_TIM_IS_TIM_COUNTING_DOWN(&Encoder_Handle));
}
void Encoder::setDirection(bool inputDirection) {
	direction_ = inputDirection;

	if (direction_)
		setSpeedCommand(-getSpeedCommand());
	else
		setSpeedCommand(abs(getSpeedCommand()));
}
//*****************************************************************************
void Encoder::setCount(int32_t count32)
{
    if (count32 < 0)
    {
        overflows_ = count32 / ((int32_t)0x10000) - 1;
    }
    else
    {
        overflows_ = count32 / ((int32_t)0x10000);
    }

    uint16_t counter = (uint16_t)(count32 - overflows_*0x10000);
    prev_counter_ = counter;

    __HAL_TIM_SET_COUNTER(&Encoder_Handle, counter);
}

float Encoder::getSpeed(void)
{
	return speed_;
}
void Encoder::setSpeed(float inputSpeed)
{
	speed_ = inputSpeed;
}
float Encoder::getSpeedCommand(void)
{
	return speedCommand_;
}
void Encoder::setSpeedCommand(float inputSpeedCommand)
{
	speedCommand_ = inputSpeedCommand;
}
float Encoder::getSpeedError(void)
{
	return speedError_;
}
void Encoder::setSpeedError(float inputSpeedError)
{
	speedError_ = inputSpeedError;
}
float Encoder::getDistance(void)
{
	return(getRevolution() * 3.145f * Belt_Pully_Diameter / Inches_Per_Meter * 100.0f * 4.0f);
}
float Encoder::getRevolution(void)
{
	return(((float)getCount() / Pulses_Per_Revolution / Motor_Gear_Ratio));
}

void Encoder::Error_Handler(void)
{
  /* Turn LED1 on */
  BSP_LED_On(LED1);
  while(1)
  {
  }
}
