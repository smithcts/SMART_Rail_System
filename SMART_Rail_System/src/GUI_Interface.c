/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2004  SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

***** emWin - Graphical user interface for embedded applications *****
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : WIDGET_SimpleButton.c
Purpose     : Example demonstrating the use of a BUTTON widget
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/
#include "main.h"

int PULSE1_VALUE = 200;
int PWM_START(int PULSE);
static void PWM_STOP();
static void Error_Handler(void);
TIM_HandleTypeDef MotorPWM;
TIM_OC_InitTypeDef sConfig;
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*******************************************************************
*
*       static code
*
********************************************************************
*/
/*******************************************************************
*
*       _DemoButton
*/
static void _DemoButton(void) {
  static BUTTON_Handle hB_Stop, hB_Forward, hB_Reverse, hB_Increase, hB_Decrease;

  //
  // Let window manager handle the button
  //
  while(1){

  //
  // Create the button and set text
  //
  hB_Stop = BUTTON_CreateEx(50, 120, 100, 40, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
  BUTTON_SetText(hB_Stop, "Stop");

  hB_Forward = BUTTON_CreateEx(50, 80, 100, 40, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
  BUTTON_SetText(hB_Forward, "Forward");

  hB_Reverse = BUTTON_CreateEx(50, 160, 100, 40, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
  BUTTON_SetText(hB_Reverse, "Reverse");

  hB_Increase = BUTTON_CreateEx(300, 80, 100, 40, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
  BUTTON_SetText(hB_Increase, "Increase");

  hB_Decrease = BUTTON_CreateEx(300, 160, 100, 40, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON4);
  BUTTON_SetText(hB_Decrease, "Decrease");


//GUI_DispStringAt("HELLO",200,50);
  switch(GUI_WaitKey()){
	  case GUI_ID_BUTTON0:
		  	GUI_DispStringAt("Stop",200,100);
			HAL_GPIO_WritePin(GPIOI,GPIO_PIN_2,GPIO_PIN_RESET);

			PWM_STOP();


		    break;
	  case GUI_ID_BUTTON1:
		  	GUI_DispStringAt("Forward",200,120);
			HAL_GPIO_WritePin(GPIOI,GPIO_PIN_2,GPIO_PIN_SET);

			PWM_START(PULSE1_VALUE);

		  	break;
	  case GUI_ID_BUTTON2:
		  	GUI_DispStringAt("Reverse",200,140);
			HAL_GPIO_WritePin(GPIOI,GPIO_PIN_2,GPIO_PIN_RESET);

			PWM_START(PULSE1_VALUE);

		  	break;
	  case GUI_ID_BUTTON3:
		    GUI_ClearRect(300,120,400,160);
		  	GUI_DispStringAt("Increase",300,130);
		  	GUI_DispDecAt(PULSE1_VALUE,300,200,3);
		  	PULSE1_VALUE = PULSE1_VALUE + 30;

			if (HAL_TIM_PWM_GetState(&MotorPWM) == 1)
			{
				PWM_START(PULSE1_VALUE);
			}
			else
			{
				PWM_STOP();
			}

		  	break;
	  case GUI_ID_BUTTON4:
		  	GUI_DispStringAt("Decrease",300,130);
		  	GUI_DispDecAt(PULSE1_VALUE,300,200,3);
		  	PULSE1_VALUE = PULSE1_VALUE - 30;

			if (HAL_TIM_PWM_GetState(&MotorPWM) == 1)
			{
				PWM_START(PULSE1_VALUE);
			}
			else
			{
				PWM_STOP();
			}

		  	break;
	  default:
		  break;
	}
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available.");
    return;
  }
  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();
  GUI_SetColor(GUI_BLACK);
  GUI_SetFont(&GUI_Font24_ASCII);
  while (1) {
    _DemoButton();
  }
}


int PWM_START(int PULSE){
	/* Set the pulse value for channel 1 */
	sConfig.Pulse = PULSE;
	if (HAL_TIM_PWM_ConfigChannel(&MotorPWM, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	{
	/* Configuration Error */
	Error_Handler();
	}
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&MotorPWM, TIM_CHANNEL_1) != HAL_OK)
	{
	/* PWM Generation Error */
	Error_Handler();
	}

	return 0;
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
static void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED1);
  while (1)
  {
  }
}
/*************************** End of file ****************************/
