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
File        : WIDGET_GraphYT.c
Purpose     : Demonstrates the use of the GRAPH widget
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "DIALOG.h"
#include "GRAPH.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MAX_VALUE 100

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 30)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GRAPH_DATA_Handle  _ahData[3]; // Array of handles for the GRAPH_DATA objects
static GRAPH_SCALE_Handle _hScaleV;   // Handle of vertical scale
static GRAPH_SCALE_Handle _hScaleH;   // Handle of horizontal scale

static I16 _aValue[3];
static int _Stop = 0;

static GUI_COLOR _aColor[] = {GUI_RED, GUI_GREEN, GUI_LIGHTBLUE}; // Array of colors for the GRAPH_DATA objects

extern Motor motor;
extern Encoder encoder;

//
// Dialog ressource
//
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Graph widget demo",  0                ,   0,   0, 480, 272 },
  { GRAPH_CreateIndirect,     0,                   GUI_ID_GRAPH0    ,   5,   5, 265, 170 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK0    , 130, 180,  50,   0 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK1    , 130, 200,  50,   0 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK6    , 180, 180,  50,   0 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK7    , 180, 200,  50,   0 },
  { BUTTON_CreateIndirect,    "Full Screen",       GUI_ID_BUTTON0   , 240, 180,  65,  18 },
  { BUTTON_CreateIndirect,    "Start",     		   GUI_ID_BUTTON1   , 100, 225,  65,  18 },
  { BUTTON_CreateIndirect,    "Stop",       	   GUI_ID_BUTTON2   , 200, 225,  65,  18 },
  { BUTTON_CreateIndirect,    "Direction",	       GUI_ID_BUTTON3   , 300, 225,  65,  18 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT0		, 410,  20,  45,  20 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT1		, 410,  50,  45,  20 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT2		, 410,  80,  45,  20 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT3		, 410,  110,  45,  20 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT4		, 410,  140,  45,  20 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT5		, 410,  170,  45,  20 },
  { EDIT_CreateIndirect,	 0, 	   			   GUI_ID_EDIT6		, 410,  200,  45,  20 },
  { TEXT_CreateIndirect,      "motorSpeed",        0                ,  340, 20,  50,  20 },
  { TEXT_CreateIndirect,      "duty_cycle",        0                ,  340, 50,  50,  20 },
  { TEXT_CreateIndirect,      "speedCommand",      0                ,  340, 80,  50,  20 },
  { TEXT_CreateIndirect,      "motorDistance",     0                ,  340, 110,  50,  20 },
  { TEXT_CreateIndirect,      "encoderCount",      0                ,  340, 140,  50,  20 },
  { TEXT_CreateIndirect,      "motorRevolutions",  0                ,  340, 170,  50,  20 },
  { TEXT_CreateIndirect,      "speedError",        0                ,  340, 200,  50,  20 },
  { SPINBOX_CreateIndirect,  NULL,                GUI_ID_SPINBOX0	, 	10, 180,  80,  50, 0, 0, 0 },
  { SLIDER_CreateIndirect,    0,                   GUI_ID_SLIDER0   ,  280,  10,  30,  160, 8, 0x0, 0},



};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _AddValues
*
* Function description
*   This routine calculates new random values in dependence of the previous added values
*   and adds them to the GRAPH_DATA objects
*/
static void _AddValues(void) {
  unsigned i;

  for (i = 0; i < GUI_COUNTOF(_aColor); i++) {
    _aValue[0] = encoder.getSpeedCommand() - encoder.getSpeed();
    _aValue[1] = encoder.getSpeed() + 50;
    _aValue[2] = motor.getDuty();
    if (_aValue[i] > MAX_VALUE) {
      _aValue[i] = MAX_VALUE;
    } else if (_aValue[i] < -MAX_VALUE) {
      _aValue[i] = -MAX_VALUE;
    }
    GRAPH_DATA_YT_AddValue(_ahData[i], _aValue[i]);
  }
}

/*********************************************************************
*
*       _UserDraw
*
* Function description
*   This routine is called by the GRAPH object before anything is drawn
*   and after the last drawing operation.
*/
static void _UserDraw(WM_HWIN hWin, int Stage) {
  if (Stage == GRAPH_DRAW_LAST) {
    char acText[] = "Speed (cm/s)";
    GUI_RECT Rect;
    GUI_RECT RectInvalid;
    int FontSizeY;

    GUI_SetFont(&GUI_Font13_ASCII);
    FontSizeY = GUI_GetFontSizeY();
    WM_GetInsideRect(&Rect);
    WM_GetInvalidRect(hWin, &RectInvalid);
    Rect.x1 = Rect.x0 + FontSizeY;
    GUI_SetColor(GUI_YELLOW);
    GUI_DispStringInRectEx(acText, &Rect, GUI_TA_HCENTER, strlen(acText), GUI_ROTATE_CCW);
  }
}

/*********************************************************************
*
*       _ForEach
*
* Function description
*   This routine hides/shows all windows except the button, graph and scroll bar widgets
*/
static void _ForEach(WM_HWIN hWin, void * pData) {
  int Id;
  int FullScreenMode;

  FullScreenMode = *(int *)pData;
  Id = WM_GetId(hWin);
  if ((Id == GUI_ID_GRAPH0) || (Id == GUI_ID_BUTTON0) || (Id == GUI_ID_VSCROLL) || (Id == GUI_ID_HSCROLL)) {
    return;
  }
  if (FullScreenMode) {
    WM_HideWindow(hWin);
  } else {
    WM_ShowWindow(hWin);
  }
}

/*********************************************************************
*
*       _ToggleFullScreenMode
*
* Function description
*   This routine switches between full screen mode and normal mode by hiding or showing the
*   widgets of the dialog, enlarging/shrinking the graph widget and modifying some other
*   attributes of the dialog widgets.
*/
static void _ToggleFullScreenMode(WM_HWIN hDlg) {
  static int FullScreenMode;
  static GUI_RECT Rect;
  static unsigned ScalePos;
  WM_HWIN hGraph;
  WM_HWIN hButton;
  WM_HWIN hClient;
  GUI_RECT RectInside;
  int xPos, yPos;

  hGraph  = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
  hButton = WM_GetDialogItem(hDlg, GUI_ID_BUTTON0);
  FullScreenMode ^= 1;
  if (FullScreenMode) {
    //
    // Enter the full screen mode
    //
    hClient = WM_GetClientWindow(hDlg);
    BUTTON_SetText(hButton, "Back");
    WM_MoveWindow(hButton, 0, 11);
    FRAMEWIN_SetTitleVis(hDlg, 0);
    WM_GetInsideRectEx(hClient, &RectInside);
    WM_GetWindowRectEx(hGraph, &Rect);
    WM_ForEachDesc(hClient, _ForEach, &FullScreenMode); // Hide all descendants
    xPos = WM_GetWindowOrgX(hClient);
    yPos = WM_GetWindowOrgY(hClient);
    WM_SetWindowPos(hGraph, xPos, yPos, RectInside.x1, RectInside.y1);
    ScalePos = GRAPH_SCALE_SetPos(_hScaleH, RectInside.y1 - 20);
  } else {
    //
    // Return to normal mode
    //
    BUTTON_SetText(hButton, "Full Screen");
    WM_MoveWindow(hButton, 0, -11);
    WM_ForEachDesc(WM_GetClientWindow(hDlg), _ForEach, &FullScreenMode); // Show all descendants
    WM_SetWindowPos(hGraph, Rect.x0, Rect.y0, Rect.x1 - Rect.x0 + 1, Rect.y1 - Rect.y0 + 1);
    FRAMEWIN_SetTitleVis(hDlg, 1);
    GRAPH_SCALE_SetPos(_hScaleH, ScalePos);
  }
}
/*********************************************************************
*
*       _cbCallback
*
* Function description
*   Callback function of the dialog
*/
static void _cbCallback(WM_MESSAGE * pMsg) {
  unsigned i;
  int      NCode;
  int      Id;
  WM_HWIN  hDlg;
  WM_HWIN  hItem;
  EDIT_Handle hEdit;

  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:

	//
	// Init spinbox widgets
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX0);
	SPINBOX_SetSkin(hItem, SPINBOX_SKIN_FLEX);
	hEdit = SPINBOX_GetEditHandle(hItem);
	EDIT_SetDecMode(hEdit, 30, 0, 150, 0, 0);
	SPINBOX_SetEditMode(hItem, SPINBOX_EM_EDIT);
	SPINBOX_SetRange(hItem, 0, 100);
	//
	// Init edit widgets
	//
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
	EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
	EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT2);
	EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT3);
	EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT4);
	EDIT_SetDecMode(hItem,0,-200000,200000,0,0);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT5);
	EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT6);
	EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);


    hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
    //
    // Add graphs
    //
    for (i = 0; i < GUI_COUNTOF(_aColor); i++) {
      _aValue[i] = rand() % 180;
      _ahData[i] = GRAPH_DATA_YT_Create(_aColor[i], 500, 0, 0);
      GRAPH_AttachData(hItem, _ahData[i]);
    }
    //
    // Set graph attributes
    //
    GRAPH_SetGridDistY(hItem, 25);
    GRAPH_SetGridVis(hItem, 1);
    GRAPH_SetGridFixedX(hItem, 1);
    GRAPH_SetUserDraw(hItem, _UserDraw);
    //
    // Create and add vertical scale
    //
    _hScaleV = GRAPH_SCALE_Create( 35, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 10);
    GRAPH_SCALE_SetTextColor(_hScaleV, GUI_YELLOW);
    GRAPH_AttachScale(hItem, _hScaleV);
    //
    // Create and add horizontal scale
    //
    _hScaleH = GRAPH_SCALE_Create(155, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 10);
    GRAPH_SCALE_SetTextColor(_hScaleH, GUI_DARKGREEN);
    GRAPH_AttachScale(hItem, _hScaleH);
    //
    // Init check boxes
    //
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK0);
    CHECKBOX_SetText(hItem, "Stop");
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK1);
    CHECKBOX_SetText(hItem, "Grid");
    CHECKBOX_SetState(hItem, 1);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK6);
    CHECKBOX_SetText(hItem, "HScroll");
    CHECKBOX_SetState(hItem, 1);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK7);
    CHECKBOX_SetText(hItem, "VScroll");
    //
    // Init slider widgets
    //
    hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0);
    SLIDER_SetRange(hItem, 0, 180);
    SLIDER_SetValue(hItem, 50);
    SLIDER_SetNumTicks(hItem, 30);
    //
    // Init button widget
    //
    hItem = WM_GetDialogItem(hDlg, GUI_ID_BUTTON0);
    WM_SetStayOnTop(hItem, 1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
    NCode = pMsg->Data.v;                 // Notification code
    switch (NCode) {
    case WM_NOTIFICATION_CLICKED:
      switch (Id) {
      case GUI_ID_BUTTON0:
        _ToggleFullScreenMode(hDlg);
        break;
      case GUI_ID_BUTTON1:			// Start button
    	  motor.setEnable(true);
    	break;
      case GUI_ID_BUTTON2:			// Stop button
    	  motor.setEnable(false);
    	  break;
      case GUI_ID_BUTTON3:			// Direction button
    	  motor.setEnable(false);
				if (encoder.getDirection())
					encoder.setDirection(false);
				else if (!encoder.getDirection())
					encoder.setDirection(true);
		  motor.setEnable(true);
    	  break;
      }
      break;
    case WM_NOTIFICATION_VALUE_CHANGED:
      switch (Id) {
      case GUI_ID_CHECK0:
        //
        // Toggle stop mode
        //
        _Stop ^= 1;
        break;
      case GUI_ID_CHECK1:
        //
        // Toggle grid
        //
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        GRAPH_SetGridVis(hItem, CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK1)));
        break;
      case GUI_ID_CHECK6:
        //
        // Toggle horizontal scroll bar
        //
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        if (CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK6)))
        {
          GRAPH_SetVSizeX(hItem, 500);
        }
        else
        {
          GRAPH_SetVSizeX(hItem, 0);
        }
        break;
      case GUI_ID_CHECK7:
        //
        // Toggle vertical scroll bar
        //
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        if (CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK7)))
        {
          GRAPH_SetVSizeY(hItem, 300);
        }
        else
        {
          GRAPH_SetVSizeY(hItem, 0);
        }
        break;
      case GUI_ID_SLIDER0:
        //
        // Set horizontal grid spacing
        //
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        encoder.setSpeedCommand(SLIDER_GetValue(pMsg->hWinSrc));

        break;
      case GUI_ID_SPINBOX0:
          hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX0);
          encoder.setSpeedCommand(SPINBOX_GetValue(pMsg->hWinSrc));
          break;
      }
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
  WM_HWIN hDlg;
  WM_HWIN hGraph;
  WM_HWIN hItem;

  hGraph = 0;
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available.");
    return;
  }
  GUI_CURSOR_Show();
  WM_SetDesktopColor(GUI_BLACK);
  #if GUI_SUPPORT_MEMDEV
    WM_SetCreateFlags(WM_CF_MEMDEV);
  #endif
  hDlg = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbCallback, 0, 0, 0);
  while (1) {
    #ifdef WIN32
      GUI_Delay(10);
    #endif
    if (!_Stop) {
      if (!hGraph) {
        hGraph = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
      }
      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
      EDIT_SetFloatValue(hItem, encoder.getSpeed());

      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
      EDIT_SetFloatValue(hItem, motor.getDuty());

      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT2);
      EDIT_SetFloatValue(hItem, encoder.getSpeedCommand());

      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT3);
      EDIT_SetFloatValue(hItem, encoder.getDistance());

      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT4);
      EDIT_SetValue(hItem, encoder.getCount());

      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT5);
      EDIT_SetFloatValue(hItem, encoder.getRevolution());

/*      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT6);
      EDIT_SetFloatValue(hItem, encoder.getSpeedCommand() - encoder.getSpeed());*/
      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT6);
      EDIT_SetFloatValue(hItem, (float)encoder.getDirection());

      _AddValues();
    }
    GUI_Exec();
  }
}

/*************************** End of file ****************************/
