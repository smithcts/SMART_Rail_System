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
#include "main.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t GUI_Initialized = 0;
TIM_HandleTypeDef TimHandle;
TIM_HandleTypeDef MotorPWM;


TIM_OC_InitTypeDef sConfig;

uint32_t uwPrescalerValue = 0;
uint32_t uhPrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
void Update_Touchscreen(void);

void GPIO_Motor_Direction_INIT();
extern void MainTask(void);
static void PWM_STOP();
static void EXTI15_10_IRQHandler_Config(void);
static void EXTI9_5_IRQHandler_Config(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable the CPU Cache */
  // CPU_CACHE_Enable();

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

  GPIO_Motor_Direction_INIT();
  /***********************************************************/

  //This timer(TIM3) is used as an interupt to refresh the touchscreen

  /* Compute the prescaler value to have TIM3 counter clock equal to 10 KHz */
  uwPrescalerValue = (uint32_t) ((SystemCoreClock /2) / 10000) - 1;


  /* Set TIMx instance */
  TimHandle.Instance = TIM3;

  /* Initialize TIM3 peripheral as follows:
       + Period = 500 - 1
       + Prescaler = ((SystemCoreClock/2)/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period = 500 - 1;
  TimHandle.Init.Prescaler = uwPrescalerValue;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    while(1)
    {
    }
  }
	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */

	if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	{
	while(1)
		{
		}
	}


  /***********************************************************/

  /***********************************************************/

	// This timer(TIM2) is used as a PWM Output on
	uhPrescalerValue = (uint32_t) ((SystemCoreClock) / 20000000) - 1;

	/* Set TIMx instance */

	MotorPWM.Instance = TIM2;

	/* Initialize TIM2 peripheral as follows:
	   + Period =
	   + Prescaler =
	   + ClockDivision = 0
	   + Counter direction = Up
	*/
	MotorPWM.Init.Prescaler         = uhPrescalerValue;
	MotorPWM.Init.Period            = 1000;
	MotorPWM.Init.ClockDivision     = 0;
	MotorPWM.Init.CounterMode       = TIM_COUNTERMODE_UP;
	MotorPWM.Init.RepetitionCounter = 0;

	if (HAL_TIM_PWM_Init(&MotorPWM) != HAL_OK)
	{
	/* Initialization Error */
	Error_Handler();
	}

	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;


  /***********************************************************/

  /* Init the STemWin GUI Library */
  BSP_SDRAM_Init(); /* Initializes the SDRAM device */
  __HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */
  GUI_Init();

  GUI_DispStringAt("Starting...", 0, 0);

  GUI_Initialized = 1;

  /* Activate the use of memory device feature */
  WM_SetCreateFlags(WM_CF_MEMDEV);








  // Starts the MainTask() Function which is in an External .c file
  MainTask();
  for(;;);




  /* Infinite loop */
  //for(;;);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  Update_Touchscreen();
}


/**
* @brief  Update_Touchscreen.
* @param  None
* @retval None
*/
void Update_Touchscreen(void)
{
	// In the BSP.c file. It is the refresh and converting function
	TouchUpdate();

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
static void SystemClock_Config(void)
{
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
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

void GPIO_Motor_Direction_INIT(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;


  	// Enable GPIO Ports
  	__HAL_RCC_GPIOI_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	return;

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

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)

{
		int Limit_Switch_1_State = 0;
		int Limit_Switch_2_State = 0;
		Limit_Switch_1_State = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
		Limit_Switch_2_State = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_10);


		if((Limit_Switch_1_State == 1) && (HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_2) == GPIO_PIN_RESET))
		{
//		BSP_LED_On(LED1);
		PWM_STOP();
		GUI_SetColor(GUI_GREEN);
		GUI_DispStringAt("Limit 1",50,20);
		}
		else
		{
//		BSP_LED_Off(LED1);
//		GUI_SetColor(GUI_BLACK);
//		GUI_DispStringAt("TEST",50,20);
		GUI_ClearRect(50,20,110,40);
		}
		if((Limit_Switch_2_State == 1))// && (HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_2) == GPIO_PIN_SET))
		{
		GUI_DispStringAt("Limit 2",350,20);
//		BSP_LED_On(LED1);
		PWM_STOP();
		}
		else
		{
//		BSP_LED_Off(LED1);
		GUI_ClearRect(350,20,410,40);
		}
}


// Any errors will cause the LED to turn on.
static void Error_Handler(void)
{
  /* Turn LED1 on */
  BSP_LED_On(LED1);
  while (1)
  {
  }
}

static void PWM_STOP(){

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Stop(&MotorPWM, TIM_CHANNEL_1) != HAL_OK)
	{
	/* PWM Generation Error */
	Error_Handler();
	}

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
