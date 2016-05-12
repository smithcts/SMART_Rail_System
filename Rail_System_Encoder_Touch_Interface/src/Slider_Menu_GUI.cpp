/*
 * Slider_Menu_GUI.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: SMART
 */

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
File        : DIALOG_SliderColor.c
Purpose     : Example demonstrating the use of a DIALOG widget
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "main.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 15)



Motor AzimuthalMotor(Azimuthal_Motor);
Motor VerticalMotor(Vertical_Motor);
Motor ClawMotor(Claw_Motor);

/*********************************************************************
*
*       Static data, SeggerLogo
*
**********************************************************************
*/

/*********************************************************************
*
*       Static data, dialog resource
*
**********************************************************************
*/
/*********************************************************************
*
*       Dialog resource
*
* Function description
*   This table conatins the info required to create the dialog.
*   It has been created manually, but could also be created by a GUI-builder.
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Motor Control", 0,         		0,  0, 480, 272, 0},
  { TEXT_CreateIndirect,     "Azimuthal:" ,  0,          		5,  35,  60,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Vertical:", 0,             		5,  85,  60,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Claw:",  0,                		5,  135,  60,  20, TEXT_CF_LEFT },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER0,  		65,  20, 300,  40 },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER1,  		65,  70, 300,  40 },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER2, 			65, 120, 300,  40 },
  { EDIT_CreateIndirect,     NULL,     GUI_ID_EDIT0,   			370,  30,  30,  20, 0, 3 },
  { EDIT_CreateIndirect,     NULL,     GUI_ID_EDIT1,   			370,  80,  30,  20, 0, 3 },
  { EDIT_CreateIndirect,     NULL,     GUI_ID_EDIT2,   			370,  130,  30,  20, 0, 3 },
  { BUTTON_CreateIndirect,   "Forward",     GUI_ID_BUTTON4,      10, 170,  80,  30 },
  { BUTTON_CreateIndirect,   "Reverse",     GUI_ID_BUTTON5,      10, 210,  80,  30 },
  { BUTTON_CreateIndirect,   "Azimuthal",     GUI_ID_BUTTON0,   110, 200,  80,  40 },
  { BUTTON_CreateIndirect,   "Vertical",     GUI_ID_BUTTON1,    200, 200,  80,  40 },
  { BUTTON_CreateIndirect,   "Claw",     GUI_ID_BUTTON2,       	290, 200,  80,  40 },
  { BUTTON_CreateIndirect,   "Stop", GUI_ID_BUTTON3,  			380, 200, 80,  40 },
};

/*********************************************************************
*
*       Static data, colors
*
**********************************************************************
*/
static U8 _duty[3] = {0, 0, 0};  // Red, green and blue components

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _OnValueChanged
*/
static void _OnValueChanged(WM_HWIN hDlg, int Id) {
  unsigned Index;
  unsigned v;
  WM_HWIN  hSlider;
  WM_HWIN  hEdit;

  Index = 0;
  v     = 0;
  if ((Id >= GUI_ID_SLIDER0) && (Id <= GUI_ID_SLIDER2)) {
    Index = Id - GUI_ID_SLIDER0;
    //
    // SLIDER-widget has changed, update EDIT-widget
    //
    hSlider = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + Index);
    hEdit   = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + Index);
    v = SLIDER_GetValue(hSlider);
    EDIT_SetValue(hEdit, v);
  } else if ((Id >= GUI_ID_EDIT0) && (Id <= GUI_ID_EDIT2)) {
    Index = Id - GUI_ID_EDIT0;
    //
    // If EDIT-widget has changed, update SLIDER-widget
    //
    hSlider = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + Index);
    hEdit   = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + Index);
    v = EDIT_GetValue(hEdit);
    SLIDER_SetValue(hSlider, v);
  }
  _duty[Index] = v;
  //
  // At last invalidate dialog client window
  //
  WM_InvalidateWindow(WM_GetClientWindow(hDlg));
}

/*********************************************************************
*
*       _cbBkWindow
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) {



  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    GUI_SetColor(GUI_BLACK);
    GUI_SetFont(&GUI_Font24_ASCII);


  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbCallback
*/
static void _cbCallback(WM_MESSAGE * pMsg) {
  WM_HWIN hDlg;
  WM_HWIN hItem;
  int     i;
  int     NCode;
  int     Id;

  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
      for (i = 0; i < 3; i++) {
        hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + i);
        SLIDER_SetRange(hItem, 0, 100);
        SLIDER_SetValue(hItem, _duty[i]);
        //
        // Init EDIT-widgets
        //
        hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + i);
        EDIT_SetDecMode(hItem, _duty[i],   0, 255, 0, 0);
      }
      break;
    case WM_KEY:
      switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) {
        case GUI_KEY_ESCAPE:
          ;
          break;
        case GUI_KEY_ENTER:
          GUI_EndDialog(hDlg, 0);
          break;
      }
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
      NCode = pMsg->Data.v;                 // Notification code
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:      // React only if released
          if (Id == GUI_ID_OK) {            // OK Button
            GUI_EndDialog(hDlg, 0);
          }
          if (Id == GUI_ID_BUTTON3) {        // Stop Button
        	  AzimuthalMotor.dutyCycle(0);
        	  AzimuthalMotor.start();

        	  VerticalMotor.dutyCycle(0);
        	  VerticalMotor.start();

        	  ClawMotor.dutyCycle(0);
        	  ClawMotor.start();
          }
          if (Id == GUI_ID_BUTTON0) {       // Azimuthal Button
        	  VerticalMotor.dutyCycle(0);
        	  VerticalMotor.start();

        	  ClawMotor.dutyCycle(0);
        	  ClawMotor.start();

        	  AzimuthalMotor.dutyCycle(_duty[0]);
        	  AzimuthalMotor.start();
          }
          if (Id == GUI_ID_BUTTON1) {        // Vertical Button
        	  AzimuthalMotor.dutyCycle(0);
        	  AzimuthalMotor.start();

        	  ClawMotor.dutyCycle(0);
        	  ClawMotor.start();

        	  VerticalMotor.dutyCycle(_duty[1]);
        	  VerticalMotor.start();
          }
          if (Id == GUI_ID_BUTTON2) {        // Claw Button
        	  AzimuthalMotor.dutyCycle(0);
        	  AzimuthalMotor.start();

        	  VerticalMotor.dutyCycle(0);
        	  VerticalMotor.start();

        	  ClawMotor.dutyCycle(_duty[2]);
        	  ClawMotor.start();
          }
          if (Id == GUI_ID_BUTTON4) {        // Forward Button
        	  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET);
          }
          if (Id == GUI_ID_BUTTON5) {        // Reverse Button
        	  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET);
          }
          break;
        case WM_NOTIFICATION_VALUE_CHANGED: // Value has changed
          _OnValueChanged(hDlg, Id);
          break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
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
  WM_SetCreateFlags(WM_CF_MEMDEV);  // Use memory devices on all windows to avoid flicker
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available.");
    return;
  }

  AzimuthalMotor.dutyCycle(0);
  AzimuthalMotor.start();

  VerticalMotor.dutyCycle(0);
  VerticalMotor.start();

  ClawMotor.dutyCycle(0);
  ClawMotor.start();

  WM_SetCallback(WM_HBKWIN, _cbBkWindow);
  while (1) {
    GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
    GUI_Delay(1000);
  }
}

/*************************** End of file ****************************/



