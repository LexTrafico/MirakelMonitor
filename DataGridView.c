/****************************************************************************\
*
*     FILE:     DataGridView.c
*
*     PURPOSE:  ListView based Datagrid with Editable subItems
*
*     COMMENTS:
*               This source is distributed in the hope that it will be useful,
*               but WITHOUT ANY WARRANTY; without even the implied warranty of
*               MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*     FUNCTIONS:
*      EXPORTS:
*               InitDataGridView() - Initialize and register the custom control
*
*               ToDo: add export functions here
*      LOCALS:
*               Grid_GetInstanceData() - Get the Instance data associated with this instance.
*               Grid_CreateInstanceData() - Allocate the Instance data associated with this instance.
*               Grid_FreeInstanceData() - Free the instance data allocation of an instance of the Grid Control.
*               Grid_GetColumnCount() - Under any condition get the number of columns in grid.
*               ListView_GetFirstVisibleRow() - Get the row number of the first visible row.
*               ListView_GetLastVisibleRow() - Get the row number of the last visible row.
*               ListView_GetRowHeight() - Get the hight of the list view component rows.
*               ListView_GetLastVisibleColWidth() - Get the width of the last visible col.
*               ListView_GetFirstVisibleColWidth() - Get the width of the first visible col.
*               SetColor() - Set the colors used to paint controls in OnCtlColor....
*               Grid_OnCtlColorEdit() - Set the colors of the Editor's Edit Control
*				CreateListView() - Creates the List View
*				CreateCellEditor() - Creates the Cell Editor
*               Edit_CenterTextVertically() - Draw vertically centered text in editor
*               Grid_SetRowHeight() - Hack - Use image list to set row height
*               Grid_OnCreate() - Handles WM_CREATE
*               Grid_OnDestroy() - Handles WM_DESTROY
*               Grid_OnGetDlgCode() - Handles WM_GETDLGCODE sent by a dialog procedure if control is
*                                      used in a dialog
*               Grid_OnSize() - Handles WM_SIZE
*               Grid_OnSetFocus() - Handles WM_SETFOCUS
*               Grid_OnSetCursor() - The cursor changes when the mouse leaves the header, this is the
*                                     only easy indication that a column resize has been compleated.
*                                    I use the event to crop the grid back to its data and to reposition
*                                     the editor
*               Grid_OnColumnResize() - Handles the HDN_ITEMCHANGING notification due to a resize attempt
*               Grid_OnCustomDraw() - Define the look and feel of the Grid
*               Grid_OnMouseClick() - Handles mouse clicks on the ListView
*               Grid_OnNotify() - Handles notification messages for control's components
*               Grid_OnCommand() - Handles notification codes of control's components
*               ListView_Proc() - Window Procedure for the ListView
*               Editor_Proc() - Window Procedure for the Editor
*               Grid_WndProc() - Window Procedure for the Data View Grid
*
*     Copyright 2007 David MacDermot.
*
* History:
*                Aug '07 - Created
*                July '09 - Added Unicode support
*                           Improved behavior of edit box during scrolling
*                           Improved redraw of grid lines
*                           Changed Get(Set)WindowLong to Get(Set)WindowLongPtr for 64bit compatability
*
\****************************************************************************/
//#define UNICODE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "DataGridView.h"

#define ID_LISTVIEW  2000
#define ID_EDIT 2001

/****************************************************************************/
// DataGridView.c Macroe Definitions

#define NELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define Refresh(hWnd) RedrawWindow(hWnd,NULL,NULL,RDW_NOERASE|RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_UPDATENOW);

/****************************************************************************/
// DataGridView.c Structure

typedef struct _tagINSTANCEDATA {
	HINSTANCE hInstance;
	HWND hwndList;
	HWND hwndEditor;
	LVHITTESTINFO hti;
	COLORREF Editor_TxtColr;
	COLORREF Editor_BkColr;
	COLORREF Cell_AltTxtColr;
	COLORREF Cell_AltBkColr;
	COLORREF Cell_ChangedTxtColr;
	COLORREF Cell_ChangedBkColr;
	COLORREF Cell_FaultyTxtColr;
	COLORREF Cell_FaultyBkColr;
	COLORREF Editor_ChangedTxtColr;
	COLORREF Editor_ChangedBkColr;
	COLORREF Editor_FaultyTxtColr;
	COLORREF Editor_FaultyBkColr;
	BOOL fPaintByRow;
	BOOL fRowHeaders;
	BOOL fResizableHeader;
	BOOL fExtendLastCol;
	BOOL fSuppressEraseBackground;
	BOOL fsizeCol;
	CHECKCHANGED pfnChangedCB;
} INSTANCEDATA, *LPINSTANCEDATA;

/****************************************************************************/
// DataGridView.c Internal Globals

LPINSTANCEDATA g_lpInst;
TCHAR g_szClassName[] = TEXT("DGridVwClass");

/****************************************************************************/
// DataGridView.c Function prototypes

static LRESULT CALLBACK Grid_WndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK Editor_Proc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK ListView_Proc(HWND, UINT, WPARAM, LPARAM);

/****************************************************************************
*
*     FUNCTION: Grid_GetInstanceData
*
*     PURPOSE:  Get the Instance data associated with this instance.
*
*     PARAMS:   HWND hGrid - Handle to Current instance
*               LPINSTANCEDATA *ppInstanceData - pointer to the address of an INSTANCEDATA struct
*
*     RETURNS:  BOOL - TRUE if successful
*
* History:
*                August '07 - Created
*
\****************************************************************************/

static BOOL Grid_GetInstanceData(HWND hGrid, LPINSTANCEDATA * ppInstanceData)
{
	*ppInstanceData = (LPINSTANCEDATA)GetProp(hGrid, TEXT("lpInsData"));
	if (NULL != *ppInstanceData)
		return TRUE;
	return FALSE;
}

/****************************************************************************
*
*     FUNCTION: Grid_CreateInstanceData
*
*     PURPOSE:  Allocate the Instance data associated with this instance.
*
*     PARAMS:   HWND hGrid - Handle to Current instance
*               LPINSTANCEDATA pInstanceData - pointer to an INSTANCEDATA struct
*
*     RETURNS:  BOOL - TRUE if successful
*
* History:
*                August '07 - Created
*
\****************************************************************************/

static BOOL Grid_CreateInstanceData(HWND hGrid, LPINSTANCEDATA pInstanceData)
{
	LPINSTANCEDATA pInst = (LPINSTANCEDATA)malloc(sizeof(INSTANCEDATA));
	memmove(pInst, pInstanceData, sizeof(INSTANCEDATA));

	return SetProp(hGrid, TEXT("lpInsData"), pInst);
}

/****************************************************************************
*
*     FUNCTION: Grid_FreeInstanceData
*
*     PURPOSE:  Free the instance data allocation of an instance of the Grid Control.
*
*     PARAMS:   HWND hGrid - Handle to Current instance
*
*     RETURNS:  BOOL - TRUE if successful
*
* History:
*                August '07 - Created
*
\****************************************************************************/

static BOOL Grid_FreeInstanceData(HWND hGrid)
{
	LPINSTANCEDATA pInst;
	if (Grid_GetInstanceData(hGrid, &pInst))
	{
		free((LPINSTANCEDATA)pInst);
		RemoveProp(hGrid, TEXT("lpInsData"));
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************
*
*     FUNCTION: Grid_GetColumnCount
*
*     PURPOSE:  Under any condition get the number of columns in grid.
*
*     PARAMS:   HWND hwnd - Handle of Grid
*
*     RETURNS:  INT - number of columns
*
* History:
*                August '07 - Created
*
\****************************************************************************/

static INT Grid_GetColumnCount(HWND hwnd)
{
	// Method: hittest low right corner of row
	BOOL fTempRow = FALSE;
	RECT rc;
	LVHITTESTINFO ht = { 0 };

	if (0 == ListView_GetItemCount(hwnd))	// No rows yet
		// Temporarily add a row in order to get the count
	{
		LV_ITEM Lv_i = { LVIF_TEXT, 0, 0, 0, 0, TEXT("") };
		ListView_InsertItem(hwnd, &Lv_i);
		fTempRow = TRUE;
	}
	ListView_GetItemRect(hwnd, ListView_GetItemCount(hwnd) - 1, &rc, LVIR_BOUNDS);
	ht.pt.x = rc.right - 1;
	ht.pt.y = rc.bottom - 1;
	ListView_SubItemHitTest(hwnd, &ht);

	if (fTempRow)
		ListView_DeleteItem(hwnd, 0);	//Delete temp row (if any)

	return ht.iSubItem + 1;
}

/****************************************************************************
*
*     FUNCTION: ListView_GetFirstVisibleRow
*
*     PURPOSE:  Get the row number of the first visible row.
*
*     PARAMS:   HWND hwnd - Handle of ListView
*
*               PRECT lprc - pointer to rectangle (Grid Control)
*
*     RETURNS:  INT - number of last row
*
* History:
*                Sept '07 - Created
*
\****************************************************************************/

static INT ListView_GetFirstVisibleRow(HWND hList)
{
	RECT rc;
	LVHITTESTINFO hti = { 0 };
	GetClientRect(g_lpInst->hwndList, &rc);
	hti.pt.x = rc.left + 10;
	hti.pt.y = rc.top + 10;
	ListView_SubItemHitTest(hList, &hti);

	return hti.iItem + 1;
}

/****************************************************************************
*
*     FUNCTION: ListView_GetLastVisibleRow
*
*     PURPOSE:  Get the row number of the last visible row.
*
*     PARAMS:   HWND hwnd - Handle of ListView
*
*               PRECT lprc - pointer to rectangle (Grid Control)
*
*     RETURNS:  INT - number of last row
*
* History:
*                Sept '07 - Created
*
\****************************************************************************/

static INT ListView_GetLastVisibleRow(HWND hList)
{
	RECT rc;
	LVHITTESTINFO hti = { 0 };
	GetClientRect(g_lpInst->hwndList, &rc);
	hti.pt.x = rc.left + 10;
	hti.pt.y = rc.bottom - 10;
	ListView_SubItemHitTest(hList, &hti);

	return hti.iItem;
}

/****************************************************************************
*
*     FUNCTION: ListView_GetRowHeight
*
*     PURPOSE:  Get the hight of the list view component rows.
*
*     PARAMS:   HWND hwnd - Handle of ListView
*
*     RETURNS:  INT - height of first row
*
* History:
*                Sept '07 - Created
*
\****************************************************************************/

static INT ListView_GetRowHeight(HWND hList)
{
	RECT rc;
	if (ListView_GetItemRect(hList, 0, &rc, LVIR_BOUNDS))
		return rc.bottom - rc.top;
	else
		return 0;
}

/****************************************************************************
*
*     FUNCTION: ListView_GetLastVisibleColWidth
*
*     PURPOSE:  Get the width of the last visible col.
*
*     PARAMS:   HWND hwnd - Handle of ListView
*
*               PRECT lprc - pointer to rectangle (Grid Control)
*
*     RETURNS:  INT - width of last col
*
* History:
*                Sept '07 - Created
*
\****************************************************************************/

static INT ListView_GetLastVisibleColWidth(HWND hList, PRECT lprc)
{
	LVHITTESTINFO hti = { 0 };
	hti.pt.x = lprc->right;
	hti.pt.y = lprc->bottom;
	MapWindowPoints(GetParent(hList), hList, &hti.pt, 1);
	ListView_SubItemHitTest(hList, &hti);

	if (hti.iSubItem < 0)
		return 0;
	else
		return ListView_GetColumnWidth(hList, hti.iSubItem);
}

/****************************************************************************
*
*     FUNCTION: ListView_GetFirstVisibleColWidth
*
*     PURPOSE:  Get the width of the first visible col.
*
*     PARAMS:   HWND hwnd - Handle of ListView
*
*               PRECT lprc - pointer to rectangle (Grid Control)
*
*     RETURNS:  INT - width of first col
*
* History:
*                Sept '07 - Created
*
\****************************************************************************/

static INT ListView_GetFirstVisibleColWidth(HWND hList, PRECT lprc)
{
	LVHITTESTINFO hti = { 0 };
	hti.pt.x = lprc->left + 10;
	hti.pt.y = lprc->top;
	MapWindowPoints(GetParent(hList), hList, &hti.pt, 1);
	ListView_SubItemHitTest(hList, &hti);

	if (hti.iSubItem < 0)
		return 0;
	else
		return ListView_GetColumnWidth(hList, hti.iSubItem);
}

/****************************************************************************
*
*     FUNCTION: SetColor
*
*     PURPOSE:  Set the colors used to paint controls in OnCtlColor....
*
*     PARAMS:   COLORREF TxtColr - Desired text color
*               COLORREF BkColr - Desired back color
*               HDC hdc - Handle of a device context
*
*     RETURNS:  HBRUSH - A reusable brush object
*
* History:
*                August '07 - Created
*
\****************************************************************************/

HBRUSH SetColor(COLORREF TxtColr, COLORREF BkColr, HDC hdc)
{
	static HBRUSH ReUsableBrush;
	DeleteObject(ReUsableBrush);
	ReUsableBrush = CreateSolidBrush(BkColr);
	SetTextColor(hdc, TxtColr);
	SetBkColor(hdc, BkColr);
	return ReUsableBrush;
}

/****************************************************************************
*
*     FUNCTION: Grid_OnCtlColorEdit
*
*     PURPOSE:  Set the colors of the Editor's Edit Control
*
*     PARAMS:   HWND hwnd - Handle of Grid
*               HDC hdc - Handle of a device context
*               HWND hwndChild - Handle of the Edit Control to custom paint
*               INT type - CTLCOLOR_EDIT
*
*     RETURNS:  HBRUSH - A reusable brush object
*
* History:
*                August '07 - Created
*
\****************************************************************************/

HBRUSH Grid_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, INT type)
{
		//menno: set color based on state 
		if (g_lpInst->hwndEditor == hwndChild)
		{
			int c = 0;
			if (g_lpInst->pfnChangedCB != NULL)
			{
				c = g_lpInst->pfnChangedCB(g_lpInst->hti.iItem);
			}
			if (c == 1)
				return SetColor(g_lpInst->Editor_TxtColr, g_lpInst->Editor_ChangedBkColr, hdc);
			else if (c == 2)
			return SetColor(g_lpInst->Editor_TxtColr, g_lpInst->Editor_FaultyBkColr, hdc);
		else
			return SetColor(g_lpInst->Editor_TxtColr, g_lpInst->Editor_BkColr, hdc);
	}
	return NULL;
}

/****************************************************************************
*
*     FUNCTION: CreateListView
*
*     PARAMS:   HINSTANCE hInstance - handle of application instance
*               HWND hwndParent - handle of list view parent
*
*     RETURNS:  HWND - the handle of the list view
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static HWND CreateListView(HINSTANCE hInstance, HWND hwndParent)
{
	DWORD dwStyle, dwExStyle;
	HWND hwnd;

	// Create the ListView
	dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_CLIPCHILDREN;

	dwExStyle = WS_EX_LEFT;
	hwnd = CreateWindowEx(
		dwExStyle,	// ex style
		WC_LISTVIEW,	// class name - defined in commctrl.h
		TEXT(""),	// dummy text
		dwStyle,	// style
		0,	// x position
		0,	// y position
		0,	// width
		0,	// height
		hwndParent,	// parent
		(HMENU)ID_LISTVIEW,	// ID
		hInstance,	// instance
		NULL);	// no extra data

	if (!hwnd)
		return NULL;

	SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

	// Subclass ListView and save the OldProc
	SetProp(hwnd, TEXT("Wprc"), (HANDLE)GetWindowLongPtr(hwnd, GWL_WNDPROC));
	SubclassWindow(hwnd, ListView_Proc);

	return hwnd;
}

/****************************************************************************
*
*     FUNCTION: CreateCellEditor
*
*     PARAMS:   HINSTANCE hInstance - handle of application instance
*               HWND hwndParent - handle of list view parent
*
*     RETURNS:  HWND - the handle of the CellEditor
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static HWND CreateCellEditor(HINSTANCE hInstance, HWND hwndParent)
{
	DWORD dwStyle, dwExStyle;
	HWND hwnd;

	dwStyle = WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_WANTRETURN;

	dwExStyle = WS_EX_TRANSPARENT | WS_EX_LEFT;

	hwnd = CreateWindowEx(
		dwExStyle,	// ex style
		WC_EDIT,	// class name - defined in commctrl.h
		TEXT(""),	// dummy text
		dwStyle,	// style
		0,	// x position
		0,	// y position
		0,	// width
		0,	// height
		hwndParent,	// parent
		(HMENU)ID_EDIT,	// ID
		hInstance,	// instance
		NULL);	// no extra data

	if (!hwnd)
		return NULL;

	SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0L);

	// Subclass Editor and save the OldProc
	SetProp(hwnd, TEXT("Wprc"), (HANDLE)GetWindowLongPtr(hwnd, GWL_WNDPROC));
	SubclassWindow(hwnd, Editor_Proc);

	return hwnd;
}

/****************************************************************************
*
*     FUNCTION: Edit_CenterTextVertically
*
*     PARAMS:   HWND hwnd - handle of an edit control
*
*     RETURNS:  VOID
*
* History:
*                Jul '09 - Created
*
\****************************************************************************/

VOID Edit_CenterTextVertically(HWND hwnd)
{
	RECT rcTxt = { 0, 0, 0, 0 };
	RECT rcEdt = { 0, 0, 0, 0 };
	HDC hdc;

	//calculate client area height needed for a font
	hdc = GetDC(hwnd);
	DrawText(hdc, TEXT("Ky"), 2, &rcTxt, DT_CALCRECT | DT_LEFT);
	ReleaseDC(hwnd, hdc);

	// Set top and left margins
	GetClientRect(hwnd, &rcEdt);
	rcEdt.left += 4;
	rcEdt.top = ((rcEdt.bottom - (rcTxt.bottom - rcTxt.top)) / 2);

	Edit_SetRect(hwnd, &rcEdt);
}

/****************************************************************************
*
*     FUNCTION: Grid_SetRowHeight
*
*     PURPOSE:  Hack - Use image list to set row height.
*
*     PARAMS:   HWND hList - Handle to listview
*               INT iRowHeight - Desired row height
*
*     RETURNS:  VOID
*
* History:
*                Sep '07 - Created
*
\****************************************************************************/

VOID Grid_SetRowHeight(HWND hList, INT iRowHeight)
{
	static HIMAGELIST hIconList;
	ImageList_Destroy(hIconList);
	hIconList = ImageList_Create(1, iRowHeight, ILC_COLOR, 0, 1);
	ListView_SetImageList(hList, hIconList, LVSIL_SMALL);
}

/****************************************************************************
*
*     FUNCTION: Grid_OnCreate
*
*     PURPOSE:  Handles WM_CREATE
*
*     PARAMS:   HWND hwnd - handle of Control
*               LPCREATESTRUCT lpCreateStruct - structure with creation data
*
*     RETURNS:  BOOL - If an application processes this message,
*                      it should return TRUE to continue creation of the window.
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static BOOL Grid_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	INSTANCEDATA inst;

	// initialize hit test info and scroll info
	memset(&inst.hti, -1, sizeof(LVHITTESTINFO));

	// get the hInstance
	inst.hInstance = lpCreateStruct->hInstance;

	// create the ListView control
	inst.hwndList = CreateListView(lpCreateStruct->hInstance, hwnd);
	if (NULL == inst.hwndList)
		return FALSE;

	//ListView_SetExtendedListViewStyleEx(inst.hwndList, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

	// gridlines default
	ListView_SetExtendedListViewStyleEx(inst.hwndList, LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER, LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	// default ListView Colors
	inst.fPaintByRow = TRUE;
	inst.Cell_AltTxtColr = ListView_GetTextColor(inst.hwndList);
	inst.Cell_AltBkColr = ListView_GetBkColor(inst.hwndList);	//White

	// default ListView Colors for changed items
	inst.Cell_ChangedBkColr = RGB(200, 255, 200);
	inst.Cell_ChangedTxtColr = RGB(0, 55, 0);
	inst.Cell_FaultyBkColr = RGB(255, 200, 200);
	inst.Cell_FaultyTxtColr = RGB(55, 0, 0);

	inst.pfnChangedCB = (CHECKCHANGED)lpCreateStruct->lpCreateParams;

	// default ListView pseudoHeaders off
	inst.fRowHeaders = FALSE;

	inst.fExtendLastCol = FALSE;

	inst.hwndEditor = CreateCellEditor(lpCreateStruct->hInstance, hwnd);
	if (NULL == inst.hwndEditor)
		return FALSE;

	// default Cell Editor Colors
	inst.Editor_BkColr = ListView_GetBkColor(inst.hwndList);
	inst.Editor_TxtColr = ListView_GetTextColor(inst.hwndList);

	// defualt editor colors for changed items
	inst.Editor_ChangedBkColr = RGB(155, 200, 155);
	inst.Editor_ChangedTxtColr = RGB(55, 0, 0);
	inst.Editor_FaultyBkColr = RGB(200, 155, 155);
	inst.Editor_FaultyTxtColr = RGB(55, 0, 0);


	Grid_SetRowHeight(inst.hwndList, 20);

	return Grid_CreateInstanceData(hwnd, &inst);
}

/****************************************************************************
*
*     FUNCTION: Grid_OnDestroy
*
*     PURPOSE:  Handles WM_DESTROY
*
*     PARAMS:   HWND hwnd - handle of Control
*
*     RETURNS:  VOID
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static VOID Grid_OnDestroy(HWND hwnd)
{
	DestroyWindow(g_lpInst->hwndList);
	DestroyWindow(g_lpInst->hwndEditor);

	Grid_FreeInstanceData(hwnd);
	PostQuitMessage(0);
}

/****************************************************************************
*
*     FUNCTION: Grid_OnGetDlgCode
*
*     PURPOSE:  Handles WM_GETDLGCODE sent by a dialog procedure if control is
*               used in a dialog.
*
*     PARAMS:   HWND hwnd - handle of control
*               LPMSG lpmsg - If lpmsg is not NULL, it is a far pointer to an MSG
*                			  structure that contains a message that is being sent to
*                			  the control.  Keyboard messages Include WM_KEYDOWN,
*							  WM_SYSCHAR, and WM_CHAR.
*
*     RETURNS:  BOOL - TRUE if handled
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static UINT Grid_OnGetDlgCode(HWND hwnd, LPMSG lpmsg)
{
	return DLGC_WANTALLKEYS;
}

/****************************************************************************
*
*     FUNCTION: Grid_OnSize
*
*     PURPOSE:  Handles WM_SIZE
*
*     PARAMS:   HWND hwnd - handle of Control
*               UINT state - Specifies the type of resizing requested
*               INT cx - width of client area
*               INT cy - height of client area
*
*     RETURNS:  VOID
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static VOID Grid_OnSize(HWND hwnd, UINT state, INT cx, INT cy)
{
	RECT rc1, rc2;
	int iWidth;

	//menno
		// Resize parent (So the listview will remain completely visible after resize)
	SetWindowPos(hwnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	//Size ListView component to fill parent
	SetWindowPos(g_lpInst->hwndList, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	if (g_lpInst->fExtendLastCol)	//Pin last column to right side
	{
		int iCount = Grid_GetColumnCount(g_lpInst->hwndList);
		if (iCount < 1)
			return;

		ListView_GetSubItemRect(g_lpInst->hwndList, 0, iCount - 1, LVIR_BOUNDS, &rc1);
		GetClientRect(g_lpInst->hwndList, &rc2);

		iWidth = rc2.right - rc2.left - rc1.left; // - GetSystemMetrics(SM_CXVSCROLL);
		ListView_SetColumnWidth(g_lpInst->hwndList, iCount - 1, 20 > iWidth ? 20 : iWidth);

		//menno
				// On resize, the editor box should resize accordingly
				// This assumes automatic resizing of last column, I guess...
		ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc1);
		//Map the SubItem rectangle to the parent dialog so that
		//we can accurately position the Edit.
		MapWindowPoints(g_lpInst->hwndList, hwnd, (LPPOINT)& rc1.left, 2);
		SetWindowPos(g_lpInst->hwndEditor, NULL, rc1.left, rc1.top, rc1.right - rc1.left, rc1.bottom - rc1.top, SWP_NOZORDER | SWP_DEFERERASE);
	}

	//GetClientRect(hwnd, &rc1);
		//rc1.right = ListView_GetColumnWidth(g_lpInst->hwndList, 0);
		//g_lpInst->fSuppressEraseBackground = TRUE;	//Supress just once
		//InvalidateRect(g_lpInst->hwndList, &rc1, FALSE);
}

/****************************************************************************
*
*     FUNCTION: Grid_OnSetFocus
*
*     PURPOSE:  Handles WM_SETFOCUS
*
*     PARAMS:   HWND hwnd - handle of Control
*               HWND hwndOldFocus - handle of Control that previously held focus
*
*     RETURNS:  VOID
*
* History:
*                Jul '09 - Created
*
\****************************************************************************/

void Grid_OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	SetFocus(g_lpInst->hwndEditor);

	//SetFocus(g_lpInst->hwndList);
	//Edit_CenterTextVertically(g_lpInst->hwndEditor);
}

/****************************************************************************
*
*     FUNCTION: Grid_OnSetCursor
*
*     PURPOSE:  The cursor changes when the mouse leaves the header, this is the
*                only easy indication that a column resize has been compleated.
*                I use the event to crop the grid back to its data and to reposition
*                the editor.
*
*     PARAMS:   HWND hwnd - handle of control
*               HWND hwndCursor - the handle of the cursor
*               UINT codeHitTest - hit test code
*               UINT msg - error message related to hit test
*
*     RETURNS:  BOOL - TRUE if handled
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

BOOL Grid_OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	RECT rc1, rc2;
	int iWidth;
	//
	// In Grid_OnColumnResize() we set g_fsizeCol when we clicked on the listview header;
	//  now we have a means of knowing that the uncaptured cursor has
	//  just slipped off the header. 
	//

	if (g_lpInst->fsizeCol)
	{
		if (g_lpInst->fExtendLastCol)	//Pin last column to right side
		{
			int iCount = Grid_GetColumnCount(g_lpInst->hwndList);
			if (iCount >= 1)
			{
				ListView_GetSubItemRect(g_lpInst->hwndList, 0, iCount - 1, LVIR_BOUNDS, &rc1);
				GetClientRect(g_lpInst->hwndList, &rc2);

				iWidth = rc2.right - rc2.left - rc1.left; // - GetSystemMetrics(SM_CXVSCROLL);
				ListView_SetColumnWidth(g_lpInst->hwndList, iCount - 1, 20 > iWidth ? 20 : iWidth);
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc1);
				//Map the SubItem rectangle to the parent dialog so that
				//we can accurately position the Edit.
				MapWindowPoints(g_lpInst->hwndList, hwnd, (LPPOINT)& rc1.left, 2);
				SetWindowPos(g_lpInst->hwndEditor, NULL, rc1.left, rc1.top, rc1.right - rc1.left, rc1.bottom - rc1.top, SWP_NOZORDER | SWP_DEFERERASE);
			}
		}

		GetClientRect(hwnd, &rc1);
		rc1.right = ListView_GetColumnWidth(g_lpInst->hwndList, 0);
		g_lpInst->fSuppressEraseBackground = TRUE;	//Supress just once
		InvalidateRect(g_lpInst->hwndList, &rc1, FALSE);

		//RePosition the editor
		ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc1);
		MapWindowPoints(g_lpInst->hwndList, hwnd, (LPPOINT)& rc1.left, 2);
		MoveWindow(g_lpInst->hwndEditor, rc1.left, rc1.top, rc1.right - rc1.left, rc1.bottom - rc1.top, TRUE);

		// Show the editor
		Edit_CenterTextVertically(g_lpInst->hwndEditor);

		ShowWindow(g_lpInst->hwndEditor, TRUE);
		SetFocus(g_lpInst->hwndEditor);
		Edit_SetSel(g_lpInst->hwndEditor, 0, -1);


		g_lpInst->fsizeCol = FALSE;
	}
	return FALSE;	//Since this is not the standard use of this event don't handle it.
}

/****************************************************************************
*
*     FUNCTION: Grid_OnColumnResize
*
*     PURPOSE:  Handles the HDN_ITEMCHANGING notification due to a resize attempt.
*
*     PARAMS:   HWND hwnd - handle of control
*               LPNMHDR pnm - pointer to the notification message header
*
*     RETURNS:  BOOL - TRUE = non resizeable, FALSE = do resize
*
* History:
*               Sep '07 - Created
*
\****************************************************************************/

BOOL Grid_OnColumnResize(HWND hwnd, LPNMHDR pnm)
{
	RECT rc1;
	if (!g_lpInst->fResizableHeader)
		return TRUE;

	ShowWindow(g_lpInst->hwndEditor, FALSE);
	g_lpInst->fsizeCol = TRUE;


	//GetClientRect(hwnd, &rc1);

	//SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc1.right, rc1.bottom));

	return FALSE;
}

/****************************************************************************
*
*     FUNCTION: Grid_OnCustomDraw
*
*     PURPOSE:  Define the look and feel of the Grid.
*
*     PARAMS:   HWND hwnd - handle of control
*               LPNMLVCUSTOMDRAW lplvcd - pointer to a custom draw message struct
*
*     RETURNS:  LRESULT - depends on custom draw notification
*
* History:
*                Sep '07 - Created
*
\****************************************************************************/

LRESULT Grid_OnCustomDraw(HWND hwnd, LPNMLVCUSTOMDRAW lplvcd)
{
	switch (lplvcd->nmcd.dwDrawStage)
	{
		//Before the paint cycle begins
		//request notifications for individual listview items
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;

		//Before an item is drawn
		//request notifications for individual listview subitems
	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	{
		int c = 0;
		if(g_lpInst->pfnChangedCB != NULL)
		{
			// Color Rows with changed values
			c = g_lpInst->pfnChangedCB(lplvcd->nmcd.dwItemSpec);
		}
		if (c == 1)
		{
			lplvcd->clrText = g_lpInst->Cell_ChangedTxtColr;
			lplvcd->clrTextBk = g_lpInst->Cell_ChangedBkColr;
		}
		else if (c == 2)
		{
			lplvcd->clrText = g_lpInst->Cell_FaultyTxtColr;
			lplvcd->clrTextBk = g_lpInst->Cell_FaultyBkColr;
		}
		// Color alternating Rows
		else if (g_lpInst->fPaintByRow && lplvcd->nmcd.dwItemSpec & 1)
		{
			lplvcd->clrText = g_lpInst->Cell_AltTxtColr;
			lplvcd->clrTextBk = g_lpInst->Cell_AltBkColr;
		}

		else if (!g_lpInst->fPaintByRow && lplvcd->iSubItem & 1)
		{
			lplvcd->clrText = g_lpInst->Cell_AltTxtColr;
			lplvcd->clrTextBk = g_lpInst->Cell_AltBkColr;
		}
		else	// Color default
		{
			lplvcd->clrText = ListView_GetTextColor(hwnd);
			lplvcd->clrTextBk = ListView_GetBkColor(hwnd);
		}
		// Color active cell
		if (lplvcd->iSubItem == g_lpInst->hti.iSubItem && lplvcd->nmcd.dwItemSpec == g_lpInst->hti.iItem)
		{
			int c = 0;
			if (g_lpInst->pfnChangedCB != NULL)
			{
				g_lpInst->pfnChangedCB(g_lpInst->hti.iItem);
			}
			// color active cell accordingly if value has been changed
			if (c == 1)
			{
				lplvcd->clrText = g_lpInst->Editor_ChangedTxtColr;
				lplvcd->clrTextBk = g_lpInst->Editor_ChangedBkColr;
			}
			else if (c == 2)
			{
				lplvcd->clrText = g_lpInst->Editor_FaultyTxtColr;
				lplvcd->clrTextBk = g_lpInst->Editor_FaultyBkColr;
			}
			else
			{
				lplvcd->clrText = g_lpInst->Editor_TxtColr;
				lplvcd->clrTextBk = g_lpInst->Editor_BkColr;
			}
		}
	}
	case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:
	{
		// Skin Column headers
//menno: standard win32 headers
			/*if (g_lpInst->fRowHeaders)
			{
				HWND hHdr;
				RECT rc;
				HDC hdc;
				HD_ITEM hdi = { 0 };
				hdi.mask = HDI_TEXT;

				hHdr = ListView_GetHeader(lplvcd->nmcd.hdr.hwndFrom);
				hdc = GetDC(hHdr);
				Header_GetItem(hHdr, lplvcd->iSubItem, &hdi);
				Header_GetItemRect(hHdr, lplvcd->iSubItem, &rc);

				if (lplvcd->iSubItem == g_lpInst->hti.iSubItem)
					DrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT | BF_SOFT);
				else
					DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT | BF_SOFT);

				ReleaseDC(hHdr, hdc);
			}*/
			// Draw Row headers
		if (lplvcd->iSubItem == 0 && g_lpInst->fRowHeaders)
		{
			static TCHAR buf[2048];
			ListView_GetSubItemRect(hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_LABEL, &lplvcd->nmcd.rc);

			lplvcd->nmcd.rc.left -= 3;
			lplvcd->nmcd.rc.top -= 1;
			lplvcd->nmcd.rc.bottom -= 1;


			if (lplvcd->nmcd.dwItemSpec == g_lpInst->hti.iItem)
			{
				//menno:different layout of active header
				DeleteObject(SelectObject(lplvcd->nmcd.hdc, CreateSolidBrush(RGB(190, 190, 190))));
				DeleteObject(SelectObject(lplvcd->nmcd.hdc, CreatePen(PS_SOLID, 1, RGB(190, 190, 190))));
				Rectangle(lplvcd->nmcd.hdc, lplvcd->nmcd.rc.left, lplvcd->nmcd.rc.top, lplvcd->nmcd.rc.right, lplvcd->nmcd.rc.bottom);
				SetBkMode(lplvcd->nmcd.hdc, TRANSPARENT);
				//DrawEdge(lplvcd->nmcd.hdc, &lplvcd->nmcd.rc, EDGE_SUNKEN, BF_RECT | BF_ADJUST | BF_MIDDLE);
			}
			else
			{
				DrawEdge(lplvcd->nmcd.hdc, &lplvcd->nmcd.rc, EDGE_RAISED, BF_RECT | BF_FLAT | BF_SOFT | BF_MIDDLE);
				SetBkMode(lplvcd->nmcd.hdc, TRANSPARENT);
			}

			ListView_GetItemText(hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, buf, sizeof buf);

			SetTextColor(lplvcd->nmcd.hdc, GetSysColor(COLOR_BTNTEXT));
			DeleteObject(SelectObject(lplvcd->nmcd.hdc, GetStockObject(DEFAULT_GUI_FONT)));
			lplvcd->nmcd.rc.left += 3;
			DrawText(lplvcd->nmcd.hdc, buf, lstrlen(buf), &lplvcd->nmcd.rc, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			SetBkMode(lplvcd->nmcd.hdc, OPAQUE);

			return CDRF_SKIPDEFAULT;
		}
		return CDRF_NEWFONT;
	}
	}
	return CDRF_DODEFAULT;
}

/****************************************************************************
*
*     FUNCTION: Grid_OnMouseClick
*
*     PURPOSE:  Handles mouse clicks on the ListView.
*
*     PARAMS:   HWND hwnd - handle of control
*               LPNMHDR pnm - pointer to the notification message header
*
*     RETURNS:  BOOL - TRUE if handled
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/


BOOL Grid_OnMouseClick(HWND hwnd, LPNMHDR pnm)
{
	static TCHAR buf[2048];
	RECT rc;
	DWORD dwpos;
	INT iSubItemOld, iItemOld;

	//Update the grid's currently selected subItem before moving on
	Edit_GetText(g_lpInst->hwndEditor, buf, sizeof buf);
	if (g_lpInst->hti.iSubItem < 2) ListView_SetItemText(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, (LPSTR)buf);
	SendMessage(GetParent(GetParent(g_lpInst->hwndList)), WM_COMMAND, MAKEWPARAM(0, DGVM_ENDEDIT), MAKELPARAM(g_lpInst->hti.iItem, g_lpInst->hti.iSubItem));

	//Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
	//ShowWindow(g_lpInst->hwndEditor, TRUE);

				//Refresh(hEdit);
	ShowWindow(g_lpInst->hwndEditor, SW_HIDE);

	//Get the mouse position at time NM_CLICK sent
	//if(NM_CLICK == pnm->code || NM_LDOWN == pnm->code)
	if (LVN_COLUMNCLICK != pnm->code)
	{
		dwpos = GetMessagePos();
		g_lpInst->hti.pt.x = GET_X_LPARAM(dwpos);
		g_lpInst->hti.pt.y = GET_Y_LPARAM(dwpos);
		MapWindowPoints(HWND_DESKTOP, hwnd, &g_lpInst->hti.pt, 1);
	}
	// correct X so clicking beside the label will also work!
	if (g_lpInst->hti.pt.x < ListView_GetColumnWidth(g_lpInst->hwndList, 0))
		g_lpInst->hti.pt.x = 5;

	//menno
	// correct Y is scrollpos is not 0, so edit box will be drawn correctly if col header is clicked
	// this is needed cause of LVN_COLUMNCLICK message, a LVN_CLICK will also be received with mouse position
	// on second thought: resizing columns is much neater when nothing else happens or can happen when the header is clicked
	if (g_lpInst->fRowHeaders && g_lpInst->hti.pt.y < ListView_GetRowHeight(g_lpInst->hwndList))//GetScrollPos(g_lpInst->hwndList, SB_VERT) != 0)
		return FALSE;
	//g_lpInst->hti.pt.y += ListView_GetRowHeight(g_lpInst->hwndList);
	//g_lpInst->hti.iItem += 1;

	ListView_SubItemHitTest(g_lpInst->hwndList, &g_lpInst->hti);


	if (g_lpInst->hti.iItem == -1 || g_lpInst->hti.iSubItem >= 2)
	{
		return FALSE;
	}

	//Get the Item, SubItem, and SubItem rectangle
	iSubItemOld = g_lpInst->hti.iSubItem >= 0 ? g_lpInst->hti.iSubItem : 1;
	//menno
			//if (g_lpInst->fRowHeaders) if(iSubItemOld == 0) iSubItemOld = 1; 
	iItemOld = g_lpInst->hti.iItem < 0 ? 0 : g_lpInst->hti.iItem;





	//Click a row header Keep old sub item
	if (g_lpInst->fRowHeaders && g_lpInst->hti.iSubItem <= 0 && LVN_COLUMNCLICK != pnm->code)
	{
		//menno changed...
		g_lpInst->hti.iSubItem = g_lpInst->fRowHeaders ? 1 : 0;
		//g_lpInst->hti.iSubItem = iSubItemOld;
	}

	//Click a column header Keep old item
	if (LVN_COLUMNCLICK == pnm->code)
	{
		if (0 == g_lpInst->hti.iSubItem && g_lpInst->fRowHeaders)
		{
			//If using Row headers then clicking col 0 places the edit box
			// over the upper left cell 
//menno: different behaviour			
			g_lpInst->hti.iItem = 0;
			//Adjust ItemOld if it is off screen
			//int iRow = ListView_GetLastVisibleRow(g_lpInst->hwndList);
			g_lpInst->hti.iSubItem = 1;
			/*if (iRow < iItemOld)
				iItemOld = iRow;
			iRow = ListView_GetFirstVisibleRow(g_lpInst->hwndList);
			if (iRow > iItemOld)
				iItemOld = iRow;
			g_lpInst->hti.iItem = iItemOld;*/
		}
		else
		{
			//Adjust ItemOld if it is off screen
			int iRow = ListView_GetLastVisibleRow(g_lpInst->hwndList);
			if (iRow < iItemOld)
				iItemOld = iRow;
			iRow = ListView_GetFirstVisibleRow(g_lpInst->hwndList);
			if (iRow > iItemOld)
				iItemOld = iRow;
			g_lpInst->hti.iItem = iItemOld;
		}
	}

	ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);

	//Map the SubItem rectangle to the parent dialog so that
	//we can accurately position the Edit.
	MapWindowPoints(g_lpInst->hwndList, hwnd, (LPPOINT)& rc.left, 2);
	SetWindowPos(g_lpInst->hwndEditor, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DEFERERASE);

	//Get the SubItem Text
	ListView_GetItemText(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, buf, sizeof buf);

	//If row headers used invalidate first column so that it will be redrawn
	if (g_lpInst->fRowHeaders)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		rc.right = ListView_GetColumnWidth(g_lpInst->hwndList, 0);
		g_lpInst->fSuppressEraseBackground = TRUE;	//Supress just once
		InvalidateRect(g_lpInst->hwndList, &rc, FALSE);
	}

	//Load and display editor
	Edit_SetText(g_lpInst->hwndEditor, buf);
	Edit_CenterTextVertically(g_lpInst->hwndEditor);

	//menno
		//if (NM_LDOWN == pnm->code)
	if (NM_DBLCLK == pnm->code || NM_LDOWN == pnm->code || NM_CLICK == pnm->code || NM_SETFOCUS == pnm->code)// || LVN_COLUMNCLICK == pnm->code)
	{
		SetFocus(g_lpInst->hwndEditor);
		Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
		ShowWindow(g_lpInst->hwndEditor, TRUE);
	}


	return TRUE;
}

/****************************************************************************
*
*     FUNCTION: Grid_OnNotify
*
*     PURPOSE:  Handles notification messages for control's components.
*
*     PARAMS:   HWND hwnd - handle of control
*               INT id - id of component
*               LPNMHDR pnm - pointer to the notification message header
*
*     RETURNS:  BOOL - TRUE if handled
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static BOOL Grid_OnNotify(HWND hwnd, INT id, LPNMHDR pnm)
{
	//
	// If this is a resizable grid, initiate column resizing
	//  resizing is concluded in the Grid_OnSetCursor event handler
	//

	if (HDN_ITEMCHANGINGA == pnm->code || HDN_ITEMCHANGINGW == pnm->code)
		return Grid_OnColumnResize(hwnd, pnm);

	//if(HDN_ITEMCHANGEDA == pnm->code)
		//Grid_SetNewSizes(hwnd, pnm);

	/*else if(LVN_COLUMNCLICK == pnm->code)// || HDN_ENDTRACK == pnm->code)
	{
		Grid_OnSetCursor(hwnd, NULL, 0, 0);
		return 0;
	}*/
	//
	// Process Custom draw
	//
	if (ID_LISTVIEW == id && pnm->code == NM_CUSTOMDRAW)
		return Grid_OnCustomDraw(hwnd, (LPNMLVCUSTOMDRAW)pnm);
	//
	// Cancel Default Label Edit
	//
//menno
	/*else if (ID_LISTVIEW == id && pnm->code == LVN_BEGINLABELEDIT)
	{
		return 0;
	}*/
	//
	// Handle mouse clicks on the grid
	//
	else if (ID_LISTVIEW == id)
	{
		if (NM_DBLCLK == pnm->code || NM_CLICK == pnm->code || NM_RDOWN == pnm->code || LVN_COLUMNCLICK == pnm->code || NM_SETFOCUS == pnm->code)
			return Grid_OnMouseClick(hwnd, pnm);
		else
			return FALSE;
	}
	else
		return FALSE;
}

/****************************************************************************
*
*     FUNCTION: ListView_Proc
*
*     PURPOSE:  Window Procedure for the ListView.
*
*    COMMENTS:  Handles Keyboard input to the listView; mostly
*                moving the editor arround.
*
*      PARAMS:  HWND   hList	- Handle of ListView
*               UINT   msg		- Which message?
*               WPARAM wParam	- message parameter
*               LPARAM lParam	- message parameter
*
*     RETURNS:  LRESULT CALLBACK - depends on message
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static LRESULT CALLBACK ListView_Proc(HWND hList, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static RECT rc, rcClient;
	static TCHAR buf[2048];
	SCROLLINFO vertBarInfo;
	int iCount, zDelta, iScrollPos, iScrollMax;
	UINT userSetting;

	HWND hParent = GetParent(hList);

	// Note: Instance data is attached to ListView's parent
	Grid_GetInstanceData(hParent, &g_lpInst);

	if (WM_DESTROY == msg)	// UnSubclass the ListView Control
	{
		SetWindowLongPtr(hList, GWL_WNDPROC, (DWORD)GetProp(hList, TEXT("Wprc")));
		RemoveProp(hList, TEXT("Wprc"));
		return 0;
	}
	else if (WM_ERASEBKGND == msg && g_lpInst->fSuppressEraseBackground)
	{	// Flicker free redraw of row headers during mouse click
		g_lpInst->fSuppressEraseBackground = FALSE;
		return TRUE;
	}
	else if (WM_VSCROLL == msg || WM_HSCROLL == msg || WM_MOUSEWHEEL == msg)
	{
		// Check if mouse button is down over the scroll bar
		//if (GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) ?
		//		VK_RBUTTON : VK_LBUTTON)) //Begin Scroll
		//{
			//Map the SubItem rectangle to the parent dialog so that
			//we can accurately position the Edit.
			//MapWindowPoints(g_lpInst->hwndList, hParent, (LPPOINT) & rc.left, 2);
			//SetWindowPos(g_lpInst->hwndEditor, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DEFERERASE);

			//ShowWindow(g_lpInst->hwndEditor, SW_HIDE);

			//Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
			//ShowWindow(g_lpInst->hwndEditor, TRUE);
		//}
		//else //End Scroll
		//{
			//Map the SubItem rectangle to the parent dialog so that
			//we can accurately position the Edit.
		MapWindowPoints(g_lpInst->hwndList, hParent, (LPPOINT)& rc.left, 2);
		SetWindowPos(g_lpInst->hwndEditor, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DEFERERASE);

		// Show Edit if it does not overlap the header or the scrollbars
		GetClientRect(g_lpInst->hwndList, &rcClient);
		if (PtInRect(&rcClient, *((LPPOINT)& rc.right)) &&
			ListView_GetFirstVisibleRow(hList) <= g_lpInst->hti.iItem)
			ShowWindow(g_lpInst->hwndEditor, TRUE);

		// Sometimes grid lines disapear so redraw them
		g_lpInst->fSuppressEraseBackground = TRUE;
		Refresh(hList);

		//Position the editor
		ShowWindow(g_lpInst->hwndEditor, FALSE);
		if (msg == WM_MOUSEWHEEL)
		{
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &userSetting, 0);
			zDelta = (GET_WHEEL_DELTA_WPARAM(wParam) / 120)*userSetting;
			GetScrollRange(g_lpInst->hwndList, SB_VERT, &iCount, &iScrollMax);
			vertBarInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
			GetScrollInfo(g_lpInst->hwndList, SB_VERT, &vertBarInfo);
			// als de scrollbar bovenaan is, en we gaan naar onder: aanpassen positie editor
			if ((vertBarInfo.nPos < 3) && zDelta < 0)// && (g_lpInst->hti.iItem - zDelta <= (vertBarInfo.nMax - (int)vertBarInfo.nPage)))
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem + zDelta, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
			// als de scrollbar op de laatste pagina is, en 
			else if ((vertBarInfo.nPos >= 3) && (g_lpInst->hti.iItem <= (vertBarInfo.nMax - (int)vertBarInfo.nPage)))
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem + zDelta, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
			else if ((vertBarInfo.nPos >= 3) && (vertBarInfo.nPos >= (vertBarInfo.nMax - 1 - (int)vertBarInfo.nPage)) && zDelta > 0)
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem + zDelta, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
			else if ((vertBarInfo.nPos >= 3) && (vertBarInfo.nPos >= (vertBarInfo.nMax - 1 - (int)vertBarInfo.nPage)))
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
			else if ((vertBarInfo.nPos >= 3) && (g_lpInst->hti.iItem > (vertBarInfo.nMax - (int)vertBarInfo.nPage)))
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem + zDelta, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
			else
				ListView_GetSubItemRect(hList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
		}
		else
		{
			static int ttrack = 0;
			vertBarInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
			GetScrollInfo(g_lpInst->hwndList, SB_VERT, &vertBarInfo);
			switch (LOWORD(wParam))
			{
			case SB_THUMBTRACK:
				ShowWindow(g_lpInst->hwndEditor, FALSE);
				//if(ttrack > HIWORD(wParam)) ttrack = g_lpInst->hti.iItem + 2 - vertBarInfo.nPos;
				//if(ttrack < HIWORD(wParam)) ttrack = g_lpInst->hti.iItem - 2 - vertBarInfo.nPos;
				//ListView_GetSubItemRect(g_lpInst->hwndList, HIWORD(wParam) + ttrack, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
				ttrack = HIWORD(wParam);
				break;
			case SB_THUMBPOSITION:
				ttrack = 0;
			case SB_LINEUP:
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem + 1, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
				break;
			case SB_LINEDOWN:
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem - 1, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
				break;
			default:
				ListView_GetSubItemRect(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
				break;
			}
		}
		MapWindowPoints(hList, hParent, (LPPOINT)& rc.left, 2);
		SetWindowPos(g_lpInst->hwndEditor, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DEFERERASE);

		//Get the SubItem Text
		ListView_GetItemText(hList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, buf, sizeof buf);

		//Load and display editor
		//Edit_SetText(g_lpInst->hwndEditor, buf);
		Edit_CenterTextVertically(g_lpInst->hwndEditor);

		Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
		if ((LOWORD(wParam) != SB_THUMBTRACK) && (rc.top > 0))
			ShowWindow(g_lpInst->hwndEditor, TRUE);
		//return 0;
	//}
	}
	else if (WM_KEYDOWN == msg)
	{
		// If the column is in resize mode when a key is pressed
		//  call the code to end column resizing first
		if (g_lpInst->fsizeCol)
			Grid_OnSetCursor(hParent, NULL, 0, 0);

		GetClientRect(hList, &rcClient);

		// iItem and iSubitem might still be initialized to -1
		//  if this is the case, select the first cell.
		if (g_lpInst->hti.iItem == -1)
			g_lpInst->hti.iItem = 0;
		if (g_lpInst->hti.iSubItem == -1)
			g_lpInst->hti.iSubItem = 0;

		// Just in case we scrolled off screen
		ListView_EnsureVisible(hList, g_lpInst->hti.iItem, FALSE);

		if (VK_TAB == wParam)	// Navigate to the next cell in this row or the next
		{
			wParam = GetKeyState(VK_SHIFT) & 0x8000 ? VK_LEFT : VK_RIGHT;
			iCount = Grid_GetColumnCount(hList);

			switch (wParam)
			{
			case VK_LEFT:
				if ((g_lpInst->fRowHeaders ? 1 : 0) >= g_lpInst->hti.iSubItem && 0 != g_lpInst->hti.iItem)
				{
					g_lpInst->hti.iSubItem = iCount - 1;
					wParam = VK_UP;
				}
				break;
			case VK_RIGHT:
				if (iCount - 1 == g_lpInst->hti.iSubItem && ListView_GetItemCount(hList) - 1 != g_lpInst->hti.iItem)
				{
					g_lpInst->hti.iSubItem = g_lpInst->fRowHeaders ? 1 : 0;
					wParam = VK_DOWN;
				}
				break;
			}
		}
		vertBarInfo.cbSize = sizeof(SCROLLINFO);
		vertBarInfo.fMask = SIF_PAGE;
		GetScrollInfo(g_lpInst->hwndList, SB_VERT, &vertBarInfo);
		switch (wParam)
		{
		case VK_HOME:
			if (g_lpInst->fRowHeaders)
				g_lpInst->hti.iSubItem = 1;
			else
				g_lpInst->hti.iSubItem = 0;
			break;
		case VK_END:
			g_lpInst->hti.iSubItem = Grid_GetColumnCount(hList) - 1;
			break;
		case VK_PRIOR:
			g_lpInst->hti.iItem = g_lpInst->hti.iItem - vertBarInfo.nPage;
			g_lpInst->hti.iItem = 0 >= g_lpInst->hti.iItem ? 0 : g_lpInst->hti.iItem;
			//menno			//g_lpInst->hti.iItem = 0;
			break;
		case VK_NEXT:
			g_lpInst->hti.iItem = (int)(g_lpInst->hti.iItem + vertBarInfo.nPage) >= (int)(ListView_GetItemCount(g_lpInst->hwndList) - 1) ? ListView_GetItemCount(hList) - 1 : g_lpInst->hti.iItem + vertBarInfo.nPage;
			//menno			//g_lpInst->hti.iItem = ListView_GetItemCount(hList) - 1;
			break;
		case VK_UP:
			g_lpInst->hti.iItem = 0 == g_lpInst->hti.iItem ? 0 : g_lpInst->hti.iItem - 1;
			break;
		case VK_DOWN:
		case VK_RETURN:
			g_lpInst->hti.iItem = (ListView_GetItemCount(g_lpInst->hwndList) - 1) == g_lpInst->hti.iItem ? g_lpInst->hti.iItem : g_lpInst->hti.iItem + 1;
			break;
		case VK_LEFT:
			if (g_lpInst->fRowHeaders)
				g_lpInst->hti.iSubItem = 1 >= g_lpInst->hti.iSubItem ? 1 : g_lpInst->hti.iSubItem - 1;
			else
				g_lpInst->hti.iSubItem = 0 == g_lpInst->hti.iSubItem ? 0 : g_lpInst->hti.iSubItem - 1;
			break;
		case VK_RIGHT:
			g_lpInst->hti.iSubItem = (Grid_GetColumnCount(hList) - 1) == g_lpInst->hti.iSubItem ? Grid_GetColumnCount(hList) - 1 : g_lpInst->hti.iSubItem + 1;
			break;
		default:
			return 0;
		}
		//Adjust Scrolls

		switch (wParam)
		{
		case VK_HOME:
			SNDMSG(hList, WM_HSCROLL, SB_TOP, 0);
			break;
		case VK_END:
			if (!PtInRect(&rcClient, *((LPPOINT)& rc.right)))
				SNDMSG(hList, WM_HSCROLL, SB_BOTTOM, 0);
			break;
		case VK_PRIOR:
			//menno			//SNDMSG(hList, WM_VSCROLL, SB_TOP, 0);
							//MessageBox(hList, "Pageup!", "Datagrid", MB_OK);
			SNDMSG(hList, WM_VSCROLL, SB_PAGEUP, 0);
			break;
		case VK_NEXT:
			//if (!PtInRect(&rcClient, *((LPPOINT) & rc.right)))
			SNDMSG(hList, WM_VSCROLL, SB_PAGEDOWN, 0);
			//menno				//SNDMSG(hList, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case VK_UP:
		{
			RECT rcHeader;
			GetClientRect(ListView_GetHeader(hList), &rcHeader);

			//Keep the edit box from overlaying the header bar
			rcClient.top += ListView_GetRowHeight(hList);

			if (!PtInRect(&rcClient, *((LPPOINT)& rc.left)))
			{
				SNDMSG(hList, WM_VSCROLL, SB_LINEUP, 0);
			}
		}
		break;
		case VK_RETURN:
		case VK_DOWN:
			rcClient.bottom -= (2 * ListView_GetRowHeight(hList));
			if (!PtInRect(&rcClient, *((LPPOINT)& rc.left)))
			{
				SNDMSG(hList, WM_VSCROLL, SB_LINEDOWN, 0);
			}
			break;
		case VK_LEFT:
			rcClient.left += ListView_GetFirstVisibleColWidth(hList, &rcClient);
			if (!PtInRect(&rcClient, *((LPPOINT)& rc.right)))
			{
				SNDMSG(hList, WM_HSCROLL, SB_PAGELEFT, 0);
			}
			break;
		case VK_RIGHT:
			rcClient.right -= ListView_GetLastVisibleColWidth(hList, &rcClient);
			if (!PtInRect(&rcClient, *((LPPOINT)& rc.left)))
			{
				SNDMSG(hList, WM_HSCROLL, SB_PAGERIGHT, 0);
			}
			break;
		default:
			return 0;
		}






		//Position the editor
		ShowWindow(g_lpInst->hwndEditor, FALSE);
		ListView_GetSubItemRect(hList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, LVIR_LABEL, &rc);
		MapWindowPoints(hList, hParent, (LPPOINT)& rc.left, 2);
		SetWindowPos(g_lpInst->hwndEditor, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DEFERERASE);

		//Get the SubItem Text
		ListView_GetItemText(hList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, buf, sizeof buf);

		//Load and display editor
		Edit_SetText(g_lpInst->hwndEditor, buf);
		Edit_CenterTextVertically(g_lpInst->hwndEditor);

		Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
		ShowWindow(g_lpInst->hwndEditor, TRUE);

		//Trigger an NM_CUSTOMDRAW notification for the curent liststview item
		// in order to update the row header to show current row selection
		g_lpInst->fSuppressEraseBackground = TRUE;
		ListView_RedrawItems(hList, g_lpInst->hti.iItem - 1, g_lpInst->hti.iItem + 1);
		UpdateWindow(hList);

		return 0;
	}
	/*else if (WM_KEYUP == msg && VK_RETURN == wParam)	// Edit subItem
	{
		Edit_CenterTextVertically(g_lpInst->hwndEditor);
		Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
		ShowWindow(g_lpInst->hwndEditor, TRUE);
		SetFocus(g_lpInst->hwndEditor);
	}*/
	return CallWindowProc((WNDPROC)GetProp(hList, TEXT("Wprc")), hList, msg, wParam, lParam);
}

/****************************************************************************
*
*     FUNCTION: Editor_Proc
*
*     PURPOSE:  Window Procedure for the Editor.
*
*    COMMENTS:  SubClassing gives access to Editor's WM_CHAR and WM_KEYDOWN
*                messages.  I handle the Enter (Return) key as WM_CHAR in
*                order to disable the bell.  I handle the Esc key in WM_KEYDOWN
*                so that the default behavior (initiating WM_DESTROY) is overridden.
*                This behavior seems to take place between WM_KEYDOWN and WM_CHAR
*                stages of message processing.
*
*      PARAMS:  HWND   hEdit	- Handle of Editor
*               UINT   msg		- Which message?
*               WPARAM wParam	- message parameter
*               LPARAM lParam	- message parameter
*
*     RETURNS:  LRESULT CALLBACK - depends on message
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

static LRESULT CALLBACK Editor_Proc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static TCHAR buf[2048];
	TCHAR temp[256];

	if (WM_DESTROY == msg)	// UnSubclass the Edit Control
	{
		SetWindowLongPtr(hEdit, GWL_WNDPROC, (DWORD)GetProp(hEdit, TEXT("Wprc")));
		RemoveProp(hEdit, TEXT("Wprc"));
		return 0;
	}

	else if (msg == WM_VSCROLL || msg == WM_HSCROLL || msg == WM_MOUSEWHEEL)
	{
		CallWindowProc((WNDPROC)ListView_Proc, g_lpInst->hwndList, msg, wParam, lParam);
		return TRUE;
	}

	else if (WM_CHAR == msg && VK_RETURN == wParam)
		return TRUE;	// handle Enter (NO BELL) 

	else if (WM_KEYUP == msg)
	{
		switch (wParam)
		{
			//case VK_RETURN:
		case VK_RIGHT:
		case VK_LEFT:
			//case VK_UP:
			//case VK_PRIOR:
			//case VK_NEXT:
			//case VK_DOWN:
		case VK_TAB:
		{
			// Note: Instance data is attached to Edit's parent
			Grid_GetInstanceData(GetParent(hEdit), &g_lpInst);
			//TCHAR temp[Edit_GetTextLength(hEdit) + 1];
			Edit_GetText(hEdit, temp, sizeof temp);
			if (g_lpInst->hti.iSubItem > 2) ListView_SetItemText(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, temp);

			//menno
							// Send message to the parent control/window/process can do something when editing ends
							// The message is sent with:
							// - WPARAM HIWORD = Message to notify parent of editing ending
							// - WPARAM LOWORD = 0
							// - LPARAM LOWORD = ID (row) of this particular message for the parent to be able to id it
							// - LPARAM HIWORD = ID (column) of this particular message for the parent to be able to id it
							// Use DataGridView_GetCellValue(LOWORD(lParam), HIWORD(lParam), temp, 32); when handling this message to retrieve the appropriate cell value,
							// where temp is pointer to char, and 32 the size of that array. The value will be written to array of chars temp
							// Reason to seperate sending the string from sending the cell coordinates is that it seems to not work to send
							// a pointer to a string via only the LOWORD of wParam; this needs the width of the whole wParam.
			SendMessage(GetParent(GetParent(g_lpInst->hwndList)), WM_COMMAND, MAKEWPARAM(0, DGVM_ENDEDIT), MAKELPARAM(g_lpInst->hti.iItem, g_lpInst->hti.iSubItem));

			Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
			ShowWindow(g_lpInst->hwndEditor, TRUE);

			Refresh(hEdit);

			/*if (VK_RETURN != wParam)	//Go to next cell in edit mode
			{
				SNDMSG(g_lpInst->hwndList, WM_KEYDOWN, (WPARAM)wParam, 0L);
				SNDMSG(g_lpInst->hwndList, WM_KEYUP, (WPARAM)VK_RETURN, 0L);
			}*/
			//else
			if (wParam == VK_RETURN)
			{
				SNDMSG(g_lpInst->hwndList, WM_KEYUP, (WPARAM)VK_RETURN, 0L);
				// Redraw Editor so it doesn't dissapear
				Edit_CenterTextVertically(hEdit);

				//Focus back to listview
				SetFocus(g_lpInst->hwndList);
				return 0;
			}
		}
		}
	}
	else if (WM_KEYDOWN == msg)
	{
		switch (wParam)
		{
		case VK_ESCAPE:	// handle Escape (NO WM_DESTROY)
			Grid_GetInstanceData(GetParent(hEdit), &g_lpInst);
			//Get the SubItem Text
			ListView_GetItemText(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, buf, sizeof buf);
			//Reset the editor
			Edit_SetText(hEdit, buf);
			// Redraw Editor so it doesn't dissapear
			Edit_CenterTextVertically(hEdit);

			//Focus back to listview
			SetFocus(g_lpInst->hwndList);
			return 0;

		case VK_DOWN:	//Go to next cell in edit mode
		case VK_UP:

		case VK_RETURN:
		case VK_PRIOR:
		case VK_NEXT:
			// Note: Instance data is attached to Edit's parent
			Grid_GetInstanceData(GetParent(hEdit), &g_lpInst);
			//TCHAR temp[Edit_GetTextLength(hEdit) + 1];
			Edit_GetText(hEdit, temp, sizeof temp);
			ListView_SetItemText(g_lpInst->hwndList, g_lpInst->hti.iItem, g_lpInst->hti.iSubItem, temp);

			//menno
							// Send message to the parent control/window/process can do something when editing ends
							// The message is sent with:
							// - WPARAM HIWORD = Message to notify parent of editing ending
							// - WPARAM LOWORD = 0
							// - LPARAM LOWORD = ID (row) of this particular message for the parent to be able to id it
							// - LPARAM HIWORD = ID (column) of this particular message for the parent to be able to id it
							// Use DataGridView_GetCellValue(LOWORD(lParam), HIWORD(lParam), temp, 32); when handling this message to retrieve the appropriate cell value,
							// where temp is pointer to char, and 32 the size of that array. The value will be written to array of chars temp
							// Reason to seperate sending the string from sending the cell coordinates is that it seems to not work to send
							// a pointer to a string via only the LOWORD of wParam; this needs the width of the whole wParam.
			SendMessage(GetParent(GetParent(g_lpInst->hwndList)), WM_COMMAND, MAKEWPARAM(0, DGVM_ENDEDIT), MAKELPARAM(g_lpInst->hti.iItem, g_lpInst->hti.iSubItem));

			//Refresh(hEdit);

			SNDMSG(g_lpInst->hwndList, WM_KEYDOWN, (WPARAM)wParam, 0L);
			SNDMSG(g_lpInst->hwndList, WM_KEYUP, (WPARAM)VK_RETURN, 0L);

			SetFocus(g_lpInst->hwndEditor);
			Edit_SetSel(g_lpInst->hwndEditor, 0, -1);
			ShowWindow(g_lpInst->hwndEditor, TRUE);
			return 0;
			break;
		}
	}

	return CallWindowProc((WNDPROC)GetProp(hEdit, TEXT("Wprc")), hEdit, msg, wParam, lParam);
}

/****************************************************************************
*
*     FUNCTION: Grid_WndProc
*
*     PURPOSE:  Window Procedure for the Data View Grid.
*
*     PARAMS:   HWND   hwnd		- This window
*               UINT   msg		- Which message?
*               WPARAM wParam	- message parameter
*               LPARAM lParam	- message parameter
*
*     RETURNS:  LRESULT CALLBACK - depends on message
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

void DataGridView_GetCellValue(HWND hWnd, int row, int col, char * string, int strsize)
{
	Grid_GetInstanceData(hWnd, &g_lpInst);
	ListView_GetItemText(g_lpInst->hwndList, row, col, string, strsize);
}

void DataGridView_SetCellValue(HWND hWnd, int row, int col, const char * string)
{
	Grid_GetInstanceData(hWnd, &g_lpInst);
	ListView_SetItemText(g_lpInst->hwndList, row, col, string);
	// Get the first selected item
	// TODO: col here should check if it is a readonly column
	if (g_lpInst->hti.iItem == row && col == 1)
			Edit_SetText(g_lpInst->hwndEditor, string);
	//	itemint = SendMessage(g_lpInst->hwndList, LVM_GETNEXTITEM, itemint, LVNI_SELECTED);
	
}

int DataGridView_GetTopIndex()
{
	return ListView_GetTopIndex(g_lpInst->hwndList);
}

int DataGridView_IsItemVisible(int row)
{
	return ListView_IsItemVisible(g_lpInst->hwndList, row);
}

void Grid_OnPaint()
{
	RECT rcClient;
	GetClientRect(g_lpInst->hwndList, &rcClient);

	HDC dc = GetDC(g_lpInst->hwndList);
	HDC dcMem = CreateCompatibleDC(&dc);
	PAINTSTRUCT ps;

	HBITMAP bmMem = CreateCompatibleBitmap(dc, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	HBITMAP pbmOld = SelectObject(dcMem, bmMem);


	FillRect(dcMem, &rcClient, GetSysColor(COLOR_WINDOW));

	DefWindowProc(g_lpInst->hwndList, WM_PAINT, (WPARAM)dcMem, (LPARAM)0);

	BeginPaint(dc, &ps);
	BitBlt(dc, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, dcMem, 0, 0, SRCCOPY);
	SelectObject(dcMem, pbmOld);
	EndPaint(dc, &ps);
	ReleaseDC(g_lpInst->hwndList, dc);

	//HeaderCtrl pCtrl = GetHeaderCtrl(g_lpInst->hwndList);
	//if (::IsWindow(pCtrl->GetSafeHWnd())
	//{
	//	CRect   aHeaderRect;
	//		pCtrl->GetClientRect(&aHeaderRect);
	//		pCtrl->RedrawWindow(&aHeaderRect);
	//}
}

static LRESULT CALLBACK Grid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Update g_lpInst for this message
	if (Grid_GetInstanceData(hwnd, &g_lpInst))
	{
		if (LVM_SETITEMA == msg || LVM_SETITEMW == msg)
		{
			if (0 == ((LPLVITEM)lParam)->iItem)	// move Editor to item 0 subitem 0
				CallWindowProc((WNDPROC)ListView_Proc, g_lpInst->hwndList, WM_KEYDOWN, VK_LEFT, 0L);
		}
		// Pass along any list view specific messages transparently
		if (LVM_FIRST <= msg && msg <= LVM_FIRST + 181)
			return SNDMSG(g_lpInst->hwndList, msg, wParam, lParam);
	}
	switch (msg)
	{

		HANDLE_MSG(hwnd, WM_CTLCOLOREDIT, Grid_OnCtlColorEdit);
		HANDLE_MSG(hwnd, WM_CREATE, Grid_OnCreate);
		HANDLE_MSG(hwnd, WM_DESTROY, Grid_OnDestroy);
		HANDLE_MSG(hwnd, WM_GETDLGCODE, Grid_OnGetDlgCode);
		HANDLE_MSG(hwnd, WM_SIZE, Grid_OnSize);
		HANDLE_MSG(hwnd, WM_NOTIFY, Grid_OnNotify);
		HANDLE_MSG(hwnd, WM_SETCURSOR, Grid_OnSetCursor);
		HANDLE_MSG(hwnd, WM_SETFOCUS, Grid_OnSetFocus);
		//HANDLE_MSG(hwnd, WM_PAINT, Grid_OnPaint);




	case WM_NOTIFYFORMAT:
#ifdef UNICODE
		return NFR_UNICODE;
#else
		return NFR_ANSI;
#endif

	case DGVM_GETLISTVIEWCONTROL:
		return (LRESULT)g_lpInst->hwndList;

	case DGVM_GETEDITCONTROL:
		return (LRESULT)g_lpInst->hwndEditor;

	case DGVM_GETCOLUMNCOUNT:
		return (LRESULT)Grid_GetColumnCount(g_lpInst->hwndList);

	case DGVM_GETEDITORBACKCLR:
		return (LRESULT)g_lpInst->Editor_BkColr;

	case DGVM_SETEDITORBACKCLR:
		g_lpInst->Editor_BkColr = (COLORREF)lParam;
		return TRUE;

	case DGVM_GETEDITORTEXTCLR:
		return (LRESULT)g_lpInst->Editor_TxtColr;

	case DGVM_SETEDITORTEXTCLR:
		g_lpInst->Editor_TxtColr = (COLORREF)lParam;
		return TRUE;

	case DGVM_RESIZEABLEHEADER:
		g_lpInst->fResizableHeader = (BOOL)wParam;
		return TRUE;

	case DGVM_GETALTBACKCLR:
		return (LRESULT)g_lpInst->Cell_AltBkColr;

	case DGVM_SETALTBACKCLR:
		g_lpInst->Cell_AltBkColr = (COLORREF)lParam;
		g_lpInst->fPaintByRow = (BOOL)wParam;
		return TRUE;

	case DGVM_SETALTTXTCLR:
		g_lpInst->Cell_AltTxtColr = (COLORREF)lParam;
		return TRUE;

	case DGVM_GETALTTXTCLR:
		return (LRESULT)g_lpInst->Cell_AltTxtColr;

	case DGVM_ROWHEADERS:
		g_lpInst->fRowHeaders = (BOOL)wParam;
		if (g_lpInst->fRowHeaders && 0 == g_lpInst->hti.iSubItem)
		{
			FORWARD_WM_KEYDOWN(g_lpInst->hwndList, VK_RIGHT, 0, 0, SNDMSG);
		}
		else
		{
			//Redraw Edit box
			Edit_CenterTextVertically(g_lpInst->hwndEditor);
		}
		Refresh(g_lpInst->hwndList);
		return TRUE;

	case DGVM_SETROWHEIGHT:
		Grid_SetRowHeight(g_lpInst->hwndList, (INT)wParam);
		return TRUE;

	case DGVM_EXTENDLASTCOLUMN:
		g_lpInst->fExtendLastCol = (BOOL)wParam;
		Refresh(g_lpInst->hwndList);
		SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
		//UpdateWindow(g_lpInst->hwndList);
		//InvalidateRect(g_lpInst->hwndList, NULL, FALSE);

		return TRUE;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

/****************************************************************************
*
*     FUNCTION: InitDataGridView
*
*     PURPOSE:  Initialize and register the custom control
*
*     PARAMS:   HINSTANCE hInstance - handle of application instance
*
*     RETURNS:  BOOL TRUE if successfull
*
* History:
*                Aug '07 - Created
*
\****************************************************************************/

BOOL InitDataGridView(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ATOM aReturn;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_BYTEALIGNCLIENT;
	wcex.lpfnWndProc = (WNDPROC)Grid_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(GetStockObject(GRAY_BRUSH));	//(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = g_szClassName;
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;

	aReturn = RegisterClassEx(&wcex);

	if (0 == aReturn)
	{
		WNDCLASS wc;

		wc.style = CS_BYTEALIGNCLIENT;
		wc.lpfnWndProc = (WNDPROC)Grid_WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(GetStockObject(GRAY_BRUSH));	//(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = g_szClassName;

		aReturn = RegisterClass(&wc);
	}

	//hPopupMenuDGV = CreatePopupMenu();
	//InsertMenu(hPopupMenuDGV, 0, MF_BYPOSITION | MF_STRING, IDM_CUSTOMDB_PASTE, TEXT("Plakken"));
	//InsertMenu(hPopupMenuDGV, 1, MF_BYPOSITION | MF_STRING, CUSTOMDBIF, TEXT("Nieuw object"));
	//InsertMenu(hPopupMenuDGV, 2, MF_BYPOSITION | MF_STRING, IDM_CUSTOMDB_ALL, TEXT("Alles selecteren"));

	return aReturn;
}
