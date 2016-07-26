#include <main.h>

/* Timer handler declaration*/
TIM_HandleTypeDef    TimHandle_TIM1;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sMotorConfig;

GPIO_InitTypeDef GPIO_InitStruct;

Motor::Motor(void)
{
		duty_ = 0;
		enable_ = false;
		prescaler_ = 99;
		period_ = 99;

/*		Initialization of GPIO pins for motor direction control*/

		/*Enable GPIO Ports*/
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/*		Initialization of TIM1 for PWM signal generation to motor*/

		TIM_HANDLE_ = TimHandle_TIM1;
		TIM_HANDLE_.Instance = TIM1;

		/*##-1- Initialize TIM3 peripheral as follows:
		 + Period = 100 - 1
		 + Prescaler = 100 - 1
		 + Coreclock = 216 MHz
		 + TIM3 Clock is at 50 MHz
		 + ClockDivision = 0
		 + Counter direction = Up
		 */
		TIM_HANDLE_.Init.Prescaler         = prescaler_;
		TIM_HANDLE_.Init.Period            = period_;
		TIM_HANDLE_.Init.ClockDivision     = 0;
		TIM_HANDLE_.Init.CounterMode       = TIM_COUNTERMODE_UP;
		TIM_HANDLE_.Init.RepetitionCounter = 0;

		HAL_TIM_PWM_Init(&TIM_HANDLE_);

		/*##-2- Configure the PWM channels #########################################*/
		/* Common configuration for all channels */
		sMotorConfig.OCMode       = TIM_OCMODE_PWM1;
		sMotorConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
		sMotorConfig.OCFastMode   = TIM_OCFAST_DISABLE;
		sMotorConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
		sMotorConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		sMotorConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

		/*##-3- Start PWM signals generation #######################################*/
		/* Start channel 1 */
		sMotorConfig.Pulse = duty_;
		HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

void Motor::setEnable(bool enable) {
	enable_ = enable;
}

bool Motor::getEnable(void){
	return enable_;
}
void Motor::stop(void)
{
	HAL_TIM_PWM_Stop(&TIM_HANDLE_, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void Motor::setDirection(bool dir)
{
	dir_ = dir;
	if (dir == true) {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	} else{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	}
}

bool Motor::getDirection(void)
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
	if(duty_<0) setDirection(true);
	else setDirection(false);

	sMotorConfig.Pulse = abs(dutyInput);

	HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

int16_t Motor::getDuty(void)
{
	return duty_;
}

void Motor::Error_Handler(void)
{
	/* Turn LED1 on */
	BSP_LED_On(LED1);
	while(1)
	{
	}
}
