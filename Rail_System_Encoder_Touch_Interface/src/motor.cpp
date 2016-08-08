#include <motor.h>

/* GPIO Pin Handler for HAL_GPIO Struct*/
GPIO_InitTypeDef GPIO_Motor_Direction_Pin;

/* Timer handler declaration*/
TIM_HandleTypeDef    PWM_Motor_Timer;
/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef PWM_Motor_OutputCompare;

/* Timer handler declaration */
TIM_HandleTypeDef    Encoder_Timer;
/* Timer Encoder Configuration Structure declaration */
TIM_Encoder_InitTypeDef Encoder_OutputCompare;

Motor::Motor(void):
		prescaler_(99),
		period_(99),
		enable_(false),
		dir_(false),
		duty_(0),
		direction_(false),
		speed_(0),
		speedCommand_(0),
		speedError_(0)
{
		InitMotor();
		InitEncoder();
}
void Motor::InitMotor(void)
{
	/*		Initialization of GPIO pins for motor direction control*/
			/*Enable GPIO Ports*/
			__HAL_RCC_GPIOG_CLK_ENABLE();
			__HAL_RCC_GPIOB_CLK_ENABLE();

			GPIO_Motor_Direction_Pin.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_Motor_Direction_Pin.Pull = GPIO_NOPULL;
			GPIO_Motor_Direction_Pin.Speed = GPIO_SPEED_HIGH;
			GPIO_Motor_Direction_Pin.Pin = GPIO_PIN_6;
			HAL_GPIO_Init(GPIOG, &GPIO_Motor_Direction_Pin);
			GPIO_Motor_Direction_Pin.Pin = GPIO_PIN_4;
			HAL_GPIO_Init(GPIOB, &GPIO_Motor_Direction_Pin);

	/*		Initialization of TIM1 for PWM signal generation to motor*/

			PWM_Motor_Timer.Instance = TIM1;

			/*##-1- Initialize TIM3 peripheral as follows:
			 + Period = 100 - 1
			 + Prescaler = 100 - 1
			 + Coreclock = 216 MHz
			 + TIM3 Clock is at 50 MHz
			 + ClockDivision = 0
			 + Counter direction = Up
			 */
			PWM_Motor_Timer.Init.Prescaler         = prescaler_;
			PWM_Motor_Timer.Init.Period            = period_;
			PWM_Motor_Timer.Init.ClockDivision     = 0;
			PWM_Motor_Timer.Init.CounterMode       = TIM_COUNTERMODE_UP;
			PWM_Motor_Timer.Init.RepetitionCounter = 0;

			HAL_TIM_PWM_Init(&PWM_Motor_Timer);

			/*##-2- Configure the PWM channels #########################################*/
			/* Common configuration for all channels */
			PWM_Motor_OutputCompare.OCMode       = TIM_OCMODE_PWM1;
			PWM_Motor_OutputCompare.OCPolarity   = TIM_OCPOLARITY_HIGH;
			PWM_Motor_OutputCompare.OCFastMode   = TIM_OCFAST_DISABLE;
			PWM_Motor_OutputCompare.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
			PWM_Motor_OutputCompare.OCNIdleState = TIM_OCNIDLESTATE_RESET;
			PWM_Motor_OutputCompare.OCIdleState  = TIM_OCIDLESTATE_RESET;

			/*##-3- Start PWM signals generation #######################################*/
			/* Start channel 1 */
			PWM_Motor_OutputCompare.Pulse = duty_;
			HAL_TIM_PWM_Start(&PWM_Motor_Timer, TIM_CHANNEL_1);
}
void Motor::InitEncoder(void)
{
	  /* -1- Initialize TIM1 to handle the encoder sensor */
	  /* Initialize TIM1 peripheral as follows:
	       + Period = 65535
	       + Prescaler = 0
	       + ClockDivision = 0
	       + Counter direction = Up
	  */
	Encoder_Timer.Instance = TIM8;

	Encoder_Timer.Init.Period            = 65535;
	Encoder_Timer.Init.Prescaler         = 0;
	Encoder_Timer.Init.ClockDivision     = 0;
	Encoder_Timer.Init.CounterMode       = TIM_COUNTERMODE_UP;
	Encoder_Timer.Init.RepetitionCounter = 0;

	Encoder_OutputCompare.EncoderMode        = TIM_ENCODERMODE_TI12;

	Encoder_OutputCompare.IC1Polarity        = TIM_ICPOLARITY_RISING;
	Encoder_OutputCompare.IC1Selection       = TIM_ICSELECTION_DIRECTTI;
	Encoder_OutputCompare.IC1Prescaler       = TIM_ICPSC_DIV1;
	Encoder_OutputCompare.IC1Filter          = 0;

	Encoder_OutputCompare.IC2Polarity        = TIM_ICPOLARITY_RISING;
	Encoder_OutputCompare.IC2Selection       = TIM_ICSELECTION_DIRECTTI;
	Encoder_OutputCompare.IC2Prescaler       = TIM_ICPSC_DIV1;
	Encoder_OutputCompare.IC2Filter          = 0;

	  if(HAL_TIM_Encoder_Init(&Encoder_Timer, &Encoder_OutputCompare) != HAL_OK)
	  {
	    /* Initialization Error */
	    Error_Handler();
	  }

	  /* Start the encoder interface */
	  HAL_TIM_Encoder_Start(&Encoder_Timer, TIM_CHANNEL_ALL);
}
void Motor::setEnable(bool enable)
{
	enable_ = enable;
}

bool Motor::getEnable(void)
{
	return enable_;
}

void Motor::stop(void)
{
	setDuty(0);

	HAL_TIM_PWM_Stop(&PWM_Motor_Timer, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void Motor::setMotorDirection(bool dir)
{
	dir_ = dir;

	if (dir == true)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	}

	else
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	}
}

bool Motor::getMotorDirection(void)
{
	return dir_;
}

void Motor::setDuty(int16_t dutyInput)
{
/*	Overflow protection, duty cycle cannot exceed the period of the TIM clock*/
	int16_t maxLimit = period_ + 1;

	if (dutyInput < -maxLimit)
		duty_ = -maxLimit;

	else if (dutyInput > maxLimit)
		duty_ = maxLimit;

	else
		duty_ = dutyInput;

/*	Reads duty cycle and sets direction if duty is positive or negative*/
	if(duty_<0)
		setMotorDirection(true);

	else
		setMotorDirection(false);

	PWM_Motor_OutputCompare.Pulse = abs(dutyInput);

	HAL_TIM_PWM_ConfigChannel(&PWM_Motor_Timer, &PWM_Motor_OutputCompare, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&PWM_Motor_Timer, TIM_CHANNEL_1);

}
int16_t Motor::getDuty(void)
{
	return duty_;
}

void Motor::setCount(int32_t count32)
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

    __HAL_TIM_SET_COUNTER(&Encoder_Timer, counter);
}
int32_t Motor::getCount(void)
{
	    uint16_t counter; // 16 bit counter of timer
	    int32_t count32;  // 32 bit counter which accounts for timer overflows

	    counter =__HAL_TIM_GET_COUNTER(&Encoder_Timer);

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
void Motor::setDirection(bool inputDirection) {
	direction_ = inputDirection;

	if (direction_)
		setSpeedCommand(-getSpeedCommand());
	else
		setSpeedCommand(abs(getSpeedCommand()));
}
bool Motor::getDirection(void) {
	return (__HAL_TIM_IS_TIM_COUNTING_DOWN(&Encoder_Timer));
}
void Motor::setSpeed(float inputSpeed)
{
	speed_ = inputSpeed;
}
float Motor::getSpeed(void)
{
	return speed_;
}
void Motor::setSpeedCommand(float inputSpeedCommand)
{
	speedCommand_ = inputSpeedCommand;
}
float Motor::getSpeedCommand(void)
{
	return speedCommand_;
}
void Motor::setSpeedError(float inputSpeedError)
{
	speedError_ = inputSpeedError;
}
float Motor::getSpeedError(void)
{
	return speedError_;
}
float Motor::getDistance(void)
{
	return(getRevolution() * 3.145f * Belt_Pully_Diameter / Inches_Per_Meter * 100.0f * 4.0f);
}
float Motor::getRevolution(void)
{
	return(((float)getCount() / Pulses_Per_Revolution / Motor_Gear_Ratio));
}
void Motor::Error_Handler(void)
{
	/* Turn LED1 on */
	BSP_LED_On(LED1);
	while(1)
	{
	}
}
