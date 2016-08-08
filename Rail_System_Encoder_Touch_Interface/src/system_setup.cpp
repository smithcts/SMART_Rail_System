/*
 * system_variables.cpp
 *
 *  Created on: May 25, 2016
 *      Author: smith
 */
#include <system_setup.h>


const float Motor_Gear_Ratio = 14.0f;
const float Pulses_Per_Revolution = 48.0f;
const float Belt_Pully_Diameter = 1.019f;		// Inches
const float Inches_Per_Meter = 39.37f;

TIM_HandleTypeDef TimHandle;

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 200000000
 *            HCLK(Hz)                       = 200000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 400
 *            PLL_P                          = 2
 *            PLL_Q                          = 8
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 6
 * @param  None
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 400;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (ret != HAL_OK) {
		while (1) {;}
	}

	/* Activate the OverDrive to reach the 200 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK) {
		while (1) {;}
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
	if (ret != HAL_OK) {
		while (1) {;}
	}
}

void InitLimitPins(void)
{
	GPIO_InitTypeDef GPIO_Limit_Pin_Handle;
/*		Initialization of GPIO pins for motor direction control*/
	/*Enable GPIO Ports*/
	__HAL_RCC_GPIOF_CLK_ENABLE();

	GPIO_Limit_Pin_Handle.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Limit_Pin_Handle.Pull = GPIO_PULLDOWN;
	GPIO_Limit_Pin_Handle.Speed = GPIO_SPEED_HIGH;
	GPIO_Limit_Pin_Handle.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOF, &GPIO_Limit_Pin_Handle);
	GPIO_Limit_Pin_Handle.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOF, &GPIO_Limit_Pin_Handle);
}
void CallbackTimer(void)
{
	/* Initialize TIM3 peripheral as follows:
	 + Period = 500 - 1
	 + Prescaler = ((SystemCoreClock/2)/10000) - 1
	 + TIM3 Clock is at 50 MHz
	 + ClockDivision = 0
	 + Counter direction = Up
	 */

	TimHandle.Instance = TIM3;
	TimHandle.Init.Period = 99;
	TimHandle.Init.Prescaler = 499;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		while (1)
		{
		}
	}
	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */

	if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
		while (1)
		{
		}
	}
}
