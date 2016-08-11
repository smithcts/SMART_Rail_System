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

/* Includes ------------------------------------------------------------------*/
#include <main.h>

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Motor motor;
PID control(5.0f,2.0f,0.3f);


/* Private function prototypes -----------------------------------------------*/
extern void SystemClock_Config(void);
extern void MainTask(void);
extern void InitLimitPins(void);
extern void CallbackTimer(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{

	HAL_Init();

	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	/* Initialize Board Specific Peripherals such as TouchScreen and SDRAM*/
	BspInit();

	/* Configure LED1 */
	BSP_LED_Init(LED1);

	CallbackTimer();

	InitLimitPins();

	/***********************************************************/

	/* Init the STemWin GUI Library */
	BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */

	GUI_Init();
	GUI_SelectLayer(0);

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

	while (1)
	{
		/*Starts the MainTask() Function which is in an External .c file*/
		MainTask();
	}
}

DerivativeFilter speedFilter(0.001, 50.0f, 0.707f);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	TouchUpdate();
/*	Far Limit, Gray wire*/
	if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_10)
			&& (motor.getSpeedCommand() > 0))
	{
			motor.setSpeedCommand(0.0f);
			control.resetIntegral();
	}
/*	Near Limit, Blue wire */
	else if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9)
			&& (motor.getSpeedCommand() < 0))
	{
			motor.setSpeedCommand(0.0f);
			control.resetIntegral();
	}

	motor.setSpeed(speedFilter.calculate(motor.getDistance()));
	motor.setSpeedError(motor.getSpeedCommand() - motor.getSpeed());
	motor.setDuty(control.calculate(motor.getSpeedError(),0.001f));


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
