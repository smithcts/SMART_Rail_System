#include "main.h"

/* Timer handler declaration*/
TIM_HandleTypeDef    TimHandle_TIM1;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sMotorConfig;

Motor::Motor(void)
{
		duty_ = 0;
		prescaler_ = 99;
		period_ = 99;
		TIM_HANDLE_ = TimHandle_TIM1;

		TIM_HANDLE_.Instance = TIM1;

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

void Motor::start(void)
{
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);

}

void Motor::stop(void)
{
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	HAL_TIM_PWM_Stop(&TIM_HANDLE_, TIM_CHANNEL_1);

}

void Motor::setDuty(int16_t dutyInput)
{
	duty_ = dutyInput;

	int16_t maxLimit = period_ + 1;

	if(duty_ < -maxLimit)duty_ = -maxLimit;
	if(duty_ >  maxLimit)duty_ =  maxLimit;

	if(dutyInput < 0){
		sMotorConfig.Pulse = abs(duty_);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	}
	else
	{	/* Set the pulse value for channel 1 */
		sMotorConfig.Pulse = duty_;
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	}

	HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1);

	start();
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
