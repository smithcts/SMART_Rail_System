/**
 ******************************************************************************

 * @file    STemWin/STemWin_HelloWorld/Src/main.c
 * @author  MCD Application Team
 * @version V1.0.2
 * @date    18-November-2015
 * @brief   This file provides main program functions
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

//#define ARM_MATH_CM7
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "arm_math.h"

/* Private typedef -----------------------------------------------------------*/
TIM_HandleTypeDef TimHandle;
TIM_OC_InitTypeDef sConfig;
/* Private define ------------------------------------------------------------*/
#define kp  0.50f
#define ki  0.0f
#define kd  0.0f
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
float TempPID;

Motor motor;
Encoder encoder;

arm_pid_instance_f32 PID;
Pid pid(15.0f,0.0f,0.0f,-0.0,0.0,-100.0f,100.0f);


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
extern void MainTask(void);
static void EXTI15_10_IRQHandler_Config(void);
static void EXTI9_5_IRQHandler_Config(void);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {

	/* STM32F7xx HAL library initialization:
	 - Configure the Flash ART accelerator on ITCM interface
	 - Configure the Systick to generate an interrupt each 1 msec
	 - Set NVIC Group Priority to 4
	 - Global MSP (MCU Support Package) initialization
	 */
	HAL_Init();

	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	// Used to refresh the touch screen and to convert the STemWin screen info to HAL_GUI info
	BspInit();

	/* Configure LED1 */
	BSP_LED_Init(LED1);

	EXTI15_10_IRQHandler_Config();
	EXTI9_5_IRQHandler_Config();

	/***********************************************************/
	//This timer(TIM3) is used as an interupt to refresh the touchscreen

	/* Set TIMx instance */
	TimHandle.Instance = TIM3;

	/* Initialize TIM3 peripheral as follows:

	 + Period = 500 - 1
	 + Prescaler = ((SystemCoreClock/2)/10000) - 1
	 + TIM3 Clock is at 50 MHz
	 + ClockDivision = 0
	 + Counter direction = Up
	 */
	TimHandle.Init.Period = 99;
	TimHandle.Init.Prescaler = 499;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		while (1) {
		}
	}
	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */

	if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
		while (1) {
		}
	}

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO Ports
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


	/***********************************************************/

	/* Init the STemWin GUI Library */
	BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */

	GUI_Init();
	GUI_SelectLayer(0);

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

	while (1) {
		// Starts the MainTask() Function which is in an External .c file
		MainTask();
	}
}
/**
 * @brief  Period elapsed callback in non blocking mode
 * @param  htim: TIM handle
 * @retval None
 */


DerivativeFilter speedFilter(0.001, 50.0f, 0.707f);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	TouchUpdate();
	static float speedError;

	encoder.setSpeed(speedFilter.calculate(encoder.getDistance()));

	if (motor.getEnable()) {
		speedError = encoder.getSpeedCommand() - encoder.getSpeed();

		motor.setDuty(arm_pid_f32(&PID, speedError));
		TempPID = pid.calculate(speedError,0.001f);

//		motor.setDuty(arm_pid_f32(&PID, speedError));

	} else
		motor.setDuty(0);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (encoder.getDirection() && HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_9)){
		motor.setEnable(false);
	}
	else if (HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_10)){
		motor.setEnable(false);
	}
}
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
static void SystemClock_Config(void) {
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
		while (1) {
			;
		}
	}

	/* Activate the OverDrive to reach the 200 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK) {
		while (1) {
			;
		}
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
		while (1) {
			;
		}
	}
}
static void EXTI15_10_IRQHandler_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /* Configure PC.13 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Enable and set EXTI lines 15 to 10 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

static void EXTI9_5_IRQHandler_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /* Configure PC.13 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Enable and set EXTI lines 15 to 10 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
