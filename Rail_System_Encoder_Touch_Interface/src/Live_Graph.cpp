/*
 * Live_Graph.cpp
 *
 *  Created on: Mar 5, 2016
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
File        : VSCREEN_RealTime.c
Purpose     : Demonstrates how to use virtual screens.

              The sample requires a display size of 320 x 240 and at
              least a virtual Y size of 480 pixels. The reccomended
              color depth is 8bpp.

              Required configuration:

              #define LCD_XSIZE        320
              #define LCD_YSIZE        240
              #define LCD_VYSIZE       480
              #define LCD_BITSPERPIXEL   8

              To find the part of the code, which switches between the
              screens, please search the function GUI_SetOrg().
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "DIALOG.h"
#include "PROGBAR.h"
#include "LCDConf.h"
#include "main.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_TEMPERATURE (GUI_ID_USER + 0)

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 30)

/*********************************************************************
*
*          Static data
*
**********************************************************************
*/



//
// Resource for DialogMain (0, 0) - (319, 239)
//
static const GUI_WIDGET_CREATE_INFO _aDialogMain[] = {
  { WINDOW_CreateIndirect,    "",                    0,                  0,   0, 480, 272, 0},
  { FRAMEWIN_CreateIndirect,  "Temperature",         ID_TEMPERATURE,     5,   5, 480, 150, 0},
  { TEXT_CreateIndirect,      "Temperature 1:",      GUI_ID_TEXT0,      10, 162,  80,   0},
  { TEXT_CreateIndirect,      "Temperature 2:",      GUI_ID_TEXT1,      10, 187,  80,   0},
  { TEXT_CreateIndirect,      "Min:",                GUI_ID_TEXT2,      10, 210,  50,   0},
  { TEXT_CreateIndirect,      "Max:",                GUI_ID_TEXT3,     110, 210,  50,   0},
  { PROGBAR_CreateIndirect,   "",                    GUI_ID_PROGBAR0,   90, 160, 100,  18},
  { PROGBAR_CreateIndirect,   "",                    GUI_ID_PROGBAR1,   90, 185, 100,  18},
  { EDIT_CreateIndirect,      "",                    GUI_ID_EDIT0,      50, 210,  40,  18, 3, 3},
  { EDIT_CreateIndirect,      "",                    GUI_ID_EDIT1,     150, 210,  40,  18, 3, 3},
  { BUTTON_CreateIndirect,    "Set color",           GUI_ID_BUTTON0,   210, 185, 100,  20},
  { BUTTON_CreateIndirect,    "Start",               GUI_ID_BUTTON1,   210, 210, 100,  20},
};





//
// Dialog handles
//
WM_HWIN _hDialogMain;




//
// Pointer to original callback routine of the temperature window
//
static WM_CALLBACK * _pcbCallbackTemperature;

//
// Maximum ranges of temperature
//
static int _TempMin = 120, _TempMax = 220;

//
// Colors to be used in temperature window
//
GUI_COLOR _ColorBackGround = GUI_RED;
GUI_COLOR _ColorGrid       = GUI_DARKGRAY;
GUI_COLOR _ColorLabel      = GUI_WHITE;
GUI_COLOR _ColorTemp1      = GUI_DARKGREEN;
GUI_COLOR _ColorTemp2      = GUI_BLUE;



//
// Arrays of temperature values
//
static I16 _aTemp1[277], _aTemp2[300];

/*********************************************************************
*
*          Static code
*
**********************************************************************
*/
static float _GetRandomValue(I16 Old) {

  return encoderCount;
}

/*********************************************************************
*
*       _InitRandomData
*/
static void _InitRandomData(I16 * paY, int n) {
  int i;
  int MaxValue;

  MaxValue = _TempMax - _TempMin;
  for (i = 0; i < n; i++) {
    if (!i) {
      paY[i] = MaxValue;
    } else {
      paY[i] = _GetRandomValue(paY[i-1]);
    }
  }
}


/*********************************************************************
*
*       _UpdateTextColors
*/
static void _UpdateTextColors(WM_HWIN hDlg) {
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
  TEXT_SetTextColor(hItem, _ColorTemp1);
  hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT1);
  TEXT_SetTextColor(hItem, _ColorTemp2);
}

/*******************************************************************
*
*       _DrawGraph
*/
static void _DrawGraph(void) {
  GUI_RECT Rect;
  int      xSize;
  int      ySize;
  int      x;
  int      y;

  WM_GetClientRect(&Rect);
  xSize = Rect.x1;
  ySize = Rect.y1;
  GUI_SetBkColor(GUI_BLACK);
  GUI_ClearRect(26, 1, 302, ySize - 21);
  GUI_SetColor(_ColorGrid);
  for (y = 20; y < (ySize - 21); y += 20) {
    int yPos = ySize - 20 - y;
    GUI_DrawHLine(yPos, 26, 302);
  }
  for (x = 40; x < (xSize - 25); x += 40) {
    int xPos = x + 25;
    GUI_DrawVLine(xPos, 1, ySize - 21);
  }
  GUI_SetColor(_ColorTemp1);
  GUI_DrawGraph(_aTemp1, GUI_COUNTOF(_aTemp1), 26, ySize - 121);
  GUI_SetColor(_ColorTemp2);
  GUI_DrawGraph(_aTemp2, GUI_COUNTOF(_aTemp2), 26, ySize - 121);
}

/*********************************************************************
*
*       _LabelGraph
*/
static void _LabelGraph(void) {
  GUI_RECT Rect;
  int      x;
  int      y;
  int      xSize;
  int      ySize;

  WM_GetClientRect(&Rect);
  xSize = Rect.x1;
  ySize = Rect.y1;
  GUI_SetBkColor(_ColorBackGround);
  GUI_Clear();
  GUI_SetColor(_ColorLabel);
  GUI_SetPenSize(1);
  GUI_ClearRect(0, (ySize - 21) - ySize, (xSize - 1), (ySize - 1));
  GUI_DrawRect(25, 0, xSize, ySize - 20);
  GUI_SetFont(&GUI_Font6x8);
  for (x = 0; x < (xSize - 20); x += 40) {
    int xPos = x + 25;
    GUI_DrawVLine(xPos, (ySize - 20), (ySize - 14));
    GUI_DispDecAt(x / 40, xPos - 2, (ySize - 9), 1);
  }
  for (y = 0; y < ySize - 20; y += 20) {
    int yPos = ySize - 20 - y;
    GUI_DrawHLine(yPos, 20, 25);
    GUI_GotoXY(1, yPos - 4);
    GUI_DispDecSpace(_TempMin + y, 3);
  }
}

/*********************************************************************
*
*       _SetProgbarValue
*/
static void _SetProgbarValue(int Id, I16 Value) {
  char    acBuffer[6] = {"   "};
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(_hDialogMain, Id);
  PROGBAR_SetValue(hItem, encoderCount);
  Value = 100 - Value + _TempMin;
  acBuffer[2] = '0' + Value % 10;
  acBuffer[1] = (Value >=  10) ? '0' + (Value % 100) /  10 : ' ';
  acBuffer[0] = (Value >= 100) ? '0' + Value / 100 : ' ';
  PROGBAR_SetText(hItem, acBuffer);
}


/*********************************************************************
*
*       _cbTemperature
*
* Function description
*   Callback routine of temperature window
*/
static void _cbTemperature(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _LabelGraph();
    _DrawGraph();
    return;
  }
  if (_pcbCallbackTemperature) {
    _pcbCallbackTemperature(pMsg);
  }
}

/*********************************************************************
*
*       _cbDialogMain
*
* Function description
*   Callback routine of DialogMain
*/
static void _cbDialogMain(WM_MESSAGE * pMsg) {
  WM_HWIN hDlg;
  WM_HWIN hItem;
  int     Id;

  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_PAINT:
    break;
  case WM_INIT_DIALOG:
    //
    // Init progress bars
    //
    hItem = WM_GetDialogItem(hDlg, GUI_ID_PROGBAR0);
    WIDGET_SetEffect(hItem, &WIDGET_Effect_3D);
    _SetProgbarValue(GUI_ID_PROGBAR0, _aTemp1[GUI_COUNTOF(_aTemp1) - 1]);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_PROGBAR1);
    WIDGET_SetEffect(hItem, &WIDGET_Effect_3D);
    _SetProgbarValue(GUI_ID_PROGBAR1, 0);
    //
    // Init edit widgets
    //
    hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
    EDIT_SetDecMode(hItem, _TempMin, 0, 999, 0, 0);
    WM_DisableWindow(hItem);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
    EDIT_SetDecMode(hItem, _TempMax, 0, 999, 0, 0);
    WM_DisableWindow(hItem);
    //
    // Init temperature window *
    //
    hItem = WM_GetClientWindow(WM_GetDialogItem(hDlg, ID_TEMPERATURE));
    _pcbCallbackTemperature = WM_SetCallback(hItem, _cbTemperature);
    _UpdateTextColors(hDlg);
    break;
  case WM_NOTIFY_PARENT:
    if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) {
      Id = WM_GetId(pMsg->hWinSrc);      // Id of widget
      switch (Id) {
      case GUI_ID_BUTTON0:

        GUI_SetOrg(0, 240);
        break;
      }
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*          Public code
*
**********************************************************************
*/
/*********************************************************************
*
*          MainTask
*/
void MainTask(void) {
  int Index;

  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available.");
    return;
  }
  #if GUI_SUPPORT_MEMDEV
    WM_SetCreateFlags(WM_CF_MEMDEV);
  #endif
  WM_SetDesktopColor(GUI_BLACK);
  //
  // Initialize the temperature arrays
  //
  _InitRandomData(_aTemp1, GUI_COUNTOF(_aTemp1));
  _InitRandomData(_aTemp2, GUI_COUNTOF(_aTemp2));

  //
  // Execute the color and the temperature dialog
  //

  _hDialogMain  = GUI_CreateDialogBox(_aDialogMain,  GUI_COUNTOF(_aDialogMain),  _cbDialogMain,  WM_HBKWIN, 0, 0);
  //
  // Add new temperatures...
  //
  Index = GUI_COUNTOF(_aTemp1) - 1;

  while (1) {
    WM_HWIN hItem;
    GUI_Delay(100); // Wait a while
    //
    // Shift the color arrays
    //
    memmove(_aTemp1, _aTemp1 + 1, sizeof(_aTemp1) - 2);
    memmove(_aTemp2, _aTemp2 + 1, sizeof(_aTemp2) - 2);
    //
    // Add new values
    //
    _aTemp1[Index] = _GetRandomValue(_aTemp1[Index - 1]);
    _aTemp2[Index] = encoderCount + 100;
    //
    // Update windows
    //
    hItem = WM_GetClientWindow(WM_GetDialogItem(_hDialogMain, ID_TEMPERATURE));
    WM_InvalidateWindow(hItem);
    _SetProgbarValue(GUI_ID_PROGBAR0, _aTemp1[Index]);
    _SetProgbarValue(GUI_ID_PROGBAR1, _aTemp2[Index]);
  }
}

/*************************** End of file ****************************/



