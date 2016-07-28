/**
  ******************************************************************************
  * @file    stm32f7xx_hal_msp_template.c
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    13-November-2015
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied 
  *          to the user folder.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]

  @endverbatim
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
#include <stm32f7xx_hal.h>

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */
		/**
		  * @brief TIM MSP Initialization
		  *        This function configures the hardware resources used in this example:
		  *           - Peripheral's clock enable
		  *           - Peripheral's GPIO Configuration
		  * @param htim: TIM handle pointer
		  * @retval None
		  */
		void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
		{
		  GPIO_InitTypeDef   GPIO_InitStruct_TIM1;

		  /*##-1- Enable peripherals and GPIO Clocks #################################*/
		  /* TIMx Peripheral clock enable */
		  __TIM1_CLK_ENABLE();

		  /* Enable all GPIO Channels Clock requested */
		  __HAL_RCC_GPIOA_CLK_ENABLE();


		  /* Common configuration for all channels */
		  GPIO_InitStruct_TIM1.Mode = GPIO_MODE_AF_PP;
		  GPIO_InitStruct_TIM1.Pull = GPIO_NOPULL;
		  GPIO_InitStruct_TIM1.Speed = GPIO_SPEED_FREQ_LOW;
		  GPIO_InitStruct_TIM1.Alternate = GPIO_AF1_TIM1;
		  GPIO_InitStruct_TIM1.Pin = GPIO_PIN_8;
		  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_TIM1);
		}

		/**
		  * @brief TIM MSP Initialization
		  *        This function configures the hardware resources used in this example:
		  *           - Peripheral's clock enable
		  *           - Peripheral's GPIO Configuration
		  * @param htim: TIM handle pointer
		  * @retval None
		  */
		void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
		{
		  GPIO_InitTypeDef   GPIO_InitStruct;

		  /*##-1- Enable peripherals and GPIO Clocks #################################*/
		  /* TIM1 Peripheral clock enable */
		   __HAL_RCC_TIM8_CLK_ENABLE();

		  /* Enable GPIO Channels Clock */
		  __HAL_RCC_GPIOC_CLK_ENABLE();

		  /*##-2- Configure I/Os #####################################################*/
		  /* Common configuration for all channels */
		  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		  GPIO_InitStruct.Pull = GPIO_PULLUP;
		  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;

		  /* Channel 1 configuration */
		  GPIO_InitStruct.Pin = GPIO_PIN_6;
		  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		  /* Channel 2 configuration */
		  GPIO_InitStruct.Pin = GPIO_PIN_7;
		  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		}

		/**
		  * @brief TIM MSP Initialization
		  *        This function configures the hardware resources used in this application:
		  *           - Peripheral's clock enable
		  *           - Peripheral's GPIO Configuration
		  * @param htim: TIM handle pointer
		  * @retval None
		  */
		void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
		{
		  /*##-1- Enable peripherals and GPIO Clocks #################################*/
		  /* TIMx Peripheral clock enable */
		  __HAL_RCC_TIM3_CLK_ENABLE();

		  /*##-2- Configure the NVIC for TIMx ########################################*/
		  /* Set the TIMx priority */
		  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);

		  /* Enable the TIMx global Interrupt */
		  HAL_NVIC_EnableIRQ(TIM3_IRQn);
		}





/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
