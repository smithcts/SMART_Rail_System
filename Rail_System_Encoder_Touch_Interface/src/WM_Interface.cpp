#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "DIALOG.h"
#include "PROGBAR.h"
#include "LCDConf.h"
#include"main.h"

#define ID_AZIMUTHAL (GUI_ID_USER + 0)

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 30)

Motor AzimuthalMotor(Azimuthal_Motor);


static const GUI_WIDGET_CREATE_INFO _aDialogMain[] = {
  { FRAMEWIN_CreateIndirect,  "Azimuthal",         ID_AZIMUTHAL,     	0,   0, 480, 272, 0},
  { TEXT_CreateIndirect,      "Distance:",      	GUI_ID_TEXT0,      10, 162,  80,   0},
  { TEXT_CreateIndirect,      "Min:",                GUI_ID_TEXT2,      10, 210,  50,   0},
  { PROGBAR_CreateIndirect,   "",                    GUI_ID_PROGBAR0,   30,  30,  100,  18},
  { EDIT_CreateIndirect,      "",                    GUI_ID_EDIT0,      50, 210,  100,  18, 12, 12},
  { BUTTON_CreateIndirect,    "Set LED",           GUI_ID_BUTTON0,     200, 100, 100,  40},
  { BUTTON_CreateIndirect,    "Start",               GUI_ID_BUTTON1,   200, 140, 100,  40},
  { BUTTON_CreateIndirect,    "Stop",               GUI_ID_BUTTON2,    200, 180, 100,  40},
  { BUTTON_CreateIndirect,    "Close",               GUI_ID_BUTTON3,    320, 140, 100,  40},
  { BUTTON_CreateIndirect,    "Open",               GUI_ID_BUTTON4,    320, 180, 100,  40},
};

//
// Dialog handles
//
WM_HWIN _hDialogMain;



/*********************************************************************
*
*          Static code
*
**********************************************************************
*/


/*********************************************************************
*
*       _SetProgbarValue
*/
static void _SetProgbarValue(int Id, I16 Value) {
  char    acBuffer[6] = {"   "};
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(_hDialogMain, Id);
  PROGBAR_SetValue(hItem, Value);

  acBuffer[2] = '0' + Value % 10;
  acBuffer[1] = (Value >=  10) ? '0' + (Value % 100) /  10 : ' ';
  acBuffer[0] = (Value >= 100) ? '0' + Value / 100 : ' ';
  PROGBAR_SetText(hItem, acBuffer);
}


/*********************************************************************
*
*       _SetEditValue
*/
static void _SetEditValue(int Id, I16 Value) {

  WM_HWIN hItem;

  hItem = WM_GetDialogItem(_hDialogMain, Id);
  EDIT_SetDecMode(hItem, cnt, 0, 0xfffffff, 0, 0);
  WM_DisableWindow(hItem);


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
    _SetProgbarValue(GUI_ID_PROGBAR0, 0);
    //
    // Init edit widgets
    //

    WM_DisableWindow(hItem);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
    EDIT_SetDecMode(hItem, 20, 0, 999, 0, 0);
    WM_DisableWindow(hItem);
    //
    // Init temperature window *
    //
    hItem = WM_GetClientWindow(WM_GetDialogItem(hDlg, ID_AZIMUTHAL));

    break;
  case WM_NOTIFY_PARENT:
    if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) {
      Id = WM_GetId(pMsg->hWinSrc);      // Id of widget
      switch (Id) {
      case GUI_ID_BUTTON0:
    	  BSP_LED_On(LED1);
        break;
      case GUI_ID_BUTTON1:
    	  AzimuthalMotor.dutyCycle(50);
    	  AzimuthalMotor.start();

        break;
      case GUI_ID_BUTTON2:
    	  AzimuthalMotor.dutyCycle(0);
    	  AzimuthalMotor.start();
        break;
      case GUI_ID_BUTTON3:
    	  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_RESET);
        break;
      case GUI_ID_BUTTON4:
    	  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_SET);
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
  // Execute the color and the temperature dialog
  //

  _hDialogMain  = GUI_CreateDialogBox(_aDialogMain,  GUI_COUNTOF(_aDialogMain),  _cbDialogMain,  WM_HBKWIN, 0, 0);

  while (1) {
    WM_HWIN hItem;
    GUI_Delay(100); // Wait a while

    //
    // Update windows
    //
    hItem = WM_GetClientWindow(WM_GetDialogItem(_hDialogMain, ID_AZIMUTHAL));
    WM_InvalidateWindow(hItem);
    _SetProgbarValue(GUI_ID_PROGBAR0, cnt);
    _SetEditValue(GUI_ID_EDIT0, cnt);
  }
}
