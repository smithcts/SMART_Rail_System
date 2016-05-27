#include "main.h"

/* Timer handler declaration*/
TIM_HandleTypeDef    TimHandle_TIM10;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sMotorConfig;

Motor::Motor(void)
{

		TIM_HANDLE_ = TimHandle_TIM10;

		TIM_HANDLE_.Instance = TIM10;

		TIM_HANDLE_.Init.Prescaler         = 99;
		TIM_HANDLE_.Init.Period            = 99;
		TIM_HANDLE_.Init.ClockDivision     = 0;
		TIM_HANDLE_.Init.CounterMode       = TIM_COUNTERMODE_UP;
		TIM_HANDLE_.Init.RepetitionCounter = 0;
		if (HAL_TIM_PWM_Init(&TIM_HANDLE_) != HAL_OK)
		{
		/* Initialization Error */
		Error_Handler();
		}

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
		sMotorConfig.Pulse = 0;
		if (HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1) != HAL_OK)
		{
		/* PWM Generation Error */
		Error_Handler();
		}
}

void Motor::start(void)
{

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1) != HAL_OK)
	{
	/* PWM Generation Error */
	Error_Handler();
	}
}

void Motor::stop(void)
{
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Stop(&TIM_HANDLE_, TIM_CHANNEL_1) != HAL_OK)
	{
	/* PWM Generation Error */
	Error_Handler();
	}
}

void Motor::dutyCycle(int16_t duty)
{
	/* Set the pulse value for channel 1 */
	sMotorConfig.Pulse = duty;
	if (HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1) != HAL_OK)
	{
	/* Configuration Error */
	Error_Handler();
	}

	/* Start channel PWM channel 1 */
	if (HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1) != HAL_OK)
	{
	/* PWM Generation Error */
	Error_Handler();
	}
}
void Motor::Error_Handler(void)
{
	/* Turn LED1 on */
	BSP_LED_On(LED1);
	while(1)
	{
	}
}
