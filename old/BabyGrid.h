//BABYGRID code is copyrighted (C) 20002 by David Hillard
//
//This code must retain this copyright message
//
//Printed BABYGRID message reference and tutorial available.
//email: mudcat@mis.net for more information.

//PellesC version DWM 07-13-06
//Created macro Defs DWM 07-18-06

#ifndef ___BABYGRID___
#define ___BABYGRID___

#include <windows.h>

#define BGN_LBUTTONDOWN 0x0001
#define BGN_MOUSEMOVE   0x0002
#define BGN_OUTOFRANGE  0x0003
#define BGN_OWNERDRAW   0x0004
#define BGN_SELCHANGE   0x0005
#define BGN_ROWCHANGED  0x0006
#define BGN_COLCHANGED  0x0007
#define BGN_EDITBEGIN   0x0008
#define BGN_DELETECELL  0x0009
#define BGN_EDITEND     0x000A
#define BGN_F1          0x000B
#define BGN_F2          0x000C
#define BGN_F3          0x000D
#define BGN_F4          0x000E
#define BGN_F5          0x000F
#define BGN_F6          0x0010
#define BGN_F7          0x0011
#define BGN_F8          0x0012
#define BGN_F9          0x0013
#define BGN_F10         0x0014
#define BGN_F11         0x0015
#define BGN_F12         0x0016
#define BGN_GOTFOCUS    0x0017
#define BGN_LOSTFOCUS   0x0018
#define BGN_CELLCLICKED 0x0019

#define BGM_PROTECTCELL WM_USER + 1
#define BGM_SETPROTECT  WM_USER + 2
#define BGM_SETCELLDATA WM_USER + 3
#define BGM_GETCELLDATA WM_USER + 4
#define BGM_CLEARGRID   WM_USER + 5
#define BGM_SETGRIDDIM  WM_USER + 6
#define BGM_DELETECELL  WM_USER + 7
#define BGM_SETCURSORPOS WM_USER + 8
#define BGM_AUTOROW     WM_USER + 9
#define BGM_GETOWNERDRAWITEM WM_USER + 10
#define BGM_SETCOLWIDTH WM_USER + 11
#define BGM_SETHEADERROWHEIGHT WM_USER + 12
#define BGM_GETTYPE     WM_USER + 13
#define BGM_GETPROTECTION WM_USER + 14
#define BGM_DRAWCURSOR  WM_USER + 15
#define BGM_SETROWHEIGHT WM_USER + 16
#define BGM_SETCURSORCOLOR WM_USER + 17
#define BGM_SETPROTECTCOLOR WM_USER + 18
#define BGM_SETUNPROTECTCOLOR WM_USER + 19
#define BGM_SETROWSNUMBERED WM_USER + 20
#define BGM_SETCOLSNUMBERED WM_USER + 21 
#define BGM_SHOWHILIGHT WM_USER + 22
#define BGM_GETROWS WM_USER + 23
#define BGM_GETCOLS WM_USER + 24
#define BGM_NOTIFYROWCHANGED WM_USER + 25
#define BGM_NOTIFYCOLCHANGED WM_USER + 26
#define BGM_GETROW WM_USER + 27
#define BGM_GETCOL WM_USER + 28
#define BGM_PAINTGRID WM_USER + 29
#define BGM_GETCOLWIDTH WM_USER + 30
#define BGM_GETROWHEIGHT WM_USER + 31
#define BGM_GETHEADERROWHEIGHT WM_USER + 32
#define BGM_SETTITLEHEIGHT WM_USER + 33

#define BGM_SETHILIGHTCOLOR WM_USER + 34
#define BGM_SETHILIGHTTEXTCOLOR WM_USER + 35
#define BGM_SETEDITABLE WM_USER + 36
#define BGM_SETGRIDLINECOLOR WM_USER + 37
#define BGM_EXTENDLASTCOLUMN WM_USER + 38
#define BGM_SHOWINTEGRALROWS WM_USER + 39
#define BGM_SETELLIPSIS WM_USER + 40
#define BGM_SETCOLAUTOWIDTH WM_USER + 41
#define BGM_SETALLOWCOLRESIZE WM_USER + 42
#define BGM_SETTITLEFONT WM_USER + 43
#define BGM_SETHEADINGFONT WM_USER + 44

#define BGM_SETNUMBEROFCOLS WM_USER + 46
#define BGM_GETCHANGED WM_USER + 47
#define BGM_SETCELLCHANGED WM_USER + 48

struct _BGCELLL {
	int row;
	int col;
};

typedef struct _BGCELLL _BGCELL;
typedef struct _BGCELLL * LPBGCELL;

_BGCELL BGcell;
LPBGCELL LPBGcell;

//function forward declarations
ATOM RegisterGridClass(HINSTANCE);
static LRESULT CALLBACK GridProc(HWND, UINT, WPARAM, LPARAM);
void SetCell(LPBGCELL cell, int row, int col);

//macro defs
#define BabyGrid_AutoRow(hGrid,fSet) (int)SendMessage((hGrid),BGM_AUTOROW,fSet,0L)
#define BabyGrid_ClearGrid(hGrid) (int)SendMessage((hGrid),BGM_CLEARGRID,0L,0L)
#define BabyGrid_DeleteCell(hGrid,pCell) (int)SendMessage((hGrid),BGM_DELETECELL,(UINT)(pCell),0L)

#define BabyGrid_ExtendLastColumn(hGrid,fSet) (int)SendMessage((hGrid),BGM_EXTENDLASTCOLUMN,fSet,0L)
#define BabyGrid_GetCellData(hGrid,pCell,lpzText) (int)SendMessage((hGrid),BGM_GETCELLDATA,(UINT)(pCell),(long)(lpzText))

#define BabyGrid_GetCol(hGrid) (int)SendMessage((hGrid),BGM_GETCOL,0L,0L)
#define BabyGrid_GetColWidth(hGrid,iCol) (int)SendMessage((hGrid),BGM_GETCOLWIDTH,iCol,0L)
#define BabyGrid_GetCols(hGrid) (int)SendMessage((hGrid),BGM_GETCOLS,0L,0L)

#define BabyGrid_GetOwnerDrawItem(hGrid) (int)SendMessage((hGrid),BGM_GETOWNERDRAWITEM,0L,0L)
#define BabyGrid_GetHeaderRowHeight(hGrid) (int)SendMessage((hGrid),BGM_GETHEADERROWHEIGHT,0L,0L)
#define BabyGrid_GetProtection(hGrid,pCell) (BOOL)SendMessage((hGrid),BGM_GETPROTECTION,(UINT)(pCell),0L) 

#define BabyGrid_GetRow(hGrid) (int)SendMessage((hGrid),BGM_GETROW,0L,0L)
#define BabyGrid_GetRowHeight(hGrid) (int)SendMessage((hGrid),BGM_GETROWHEIGHT,0L,0L) 
#define BabyGrid_GetRows(hGrid) (int)SendMessage((hGrid),BGM_GETROWS,0L,0L)
#define BabyGrid_GetType(hGrid,pCell) (int)SendMessage((hGrid),BGM_GETTYPE,(UINT)(pCell),0L)

#define BabyGrid_NotifyColChanged(hGrid) (int)SendMessage((hGrid),BGM_NOTIFYCOLCHANGED,0L,0L)
#define BabyGrid_NotifyRowChanged(hGrid) (int)SendMessage((hGrid),BGM_NOTIFYROWCHANGED,0L,0L)

#define BabyGrid_PaintGrid(hGrid) (int)SendMessage((hGrid),BGM_PAINTGRID,0L,0L)
#define BabyGrid_ProtectCell(hGrid,pCell,fSet) (int)SendMessage((hGrid),BGM_PROTECTCELL,(UINT)(pCell),fSet)

#define BabyGrid_SetAllowColResize(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETALLOWCOLRESIZE,fSet,0L)
#define BabyGrid_SetCellData(hGrid,pCell,lpzText) (int)SendMessage((hGrid),BGM_SETCELLDATA,(UINT)(pCell),(long)(lpzText))

#define BabyGrid_SetColAutoWidth(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETCOLAUTOWIDTH,fSet,0L)
#define BabyGrid_SetColWidth(hGrid,iCol,nWidth) (int)SendMessage((hGrid),BGM_SETCOLWIDTH,iCol,nWidth)
#define BabyGrid_SetColsNumbered(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETCOLSNUMBERED,fSet,0L)

#define BabyGrid_SetCursorColor(hGrid, clrCrsr) (int)SendMessage((hGrid),BGM_SETCURSORCOLOR,(UINT)clrCrsr,0L)
#define BabyGrid_SetCursorPos(hGrid,iRow,iCol) (int)SendMessage((hGrid),BGM_SETCURSORPOS,iRow,iCol)

#define BabyGrid_SetEditTable(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETEDITABLE,fSet,0L)
#define BabyGrid_SetEllipsis(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETELLIPSIS,fSet,0L)

#define BabyGrid_SetGridDim(hGrid,nRows,nCols) (int)SendMessage((hGrid),BGM_SETGRIDDIM,nRows,nCols)
#define BabyGrid_SetGridLineColor(hGrid,clrGrdLine)  (int)SendMessage((hGrid),BGM_SETGRIDLINECOLOR,(UINT)clrGrdLine,0L)

#define BabyGrid_SetHeaderRowHeight(hGrid,nHeight) (int)SendMessage((hGrid),BGM_SETHEADERROWHEIGHT,nHeight,0L)
#define BabyGrid_SetHeadingFont(hGrid,hFont) (int)SendMessage((hGrid),BGM_SETHEADINGFONT,(UINT)(hFont),0L)

#define BabyGrid_SetHilightColor(hGrid, clrHilt) (int)SendMessage((hGrid),BGM_SETHILIGHTCOLOR,(UINT)clrHilt,0L)
#define BabyGrid_SetHilightTextColor(hGrid, clrHlText) (int)SendMessage((hGrid),BGM_SETHILIGHTTEXTCOLOR,(UINT)clrHlText,0L)

#define BabyGrid_SetCellChanged(hGrid,pCell,fSet) (int)SendMessage((hGrid),BGM_SETCELLCHANGED,(UINT)(pCell),fSet)

#define BabyGrid_SetProtect(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETPROTECT,fSet,0L)
#define BabyGrid_SetProtectColor(hGrid, clrProtect) (int)SendMessage((hGrid),BGM_SETPROTECTCOLOR,(UINT) clrProtect,0L)

#define BabyGrid_SetRowHeight(hGrid,iHeight) (int)SendMessage((hGrid),BGM_SETROWHEIGHT,iHeight,0L)
#define BabyGrid_SetRowsNumbered(hGrid,fSet) (int)SendMessage((hGrid),BGM_SETROWSNUMBERED,fSet,0L)

#define BabyGrid_SetTitleFont(hGrid,hFont) (int)SendMessage((hGrid),BGM_SETTITLEFONT,(UINT)(hFont),0L)
#define BabyGrid_SetTitleHeight(hGrid,iHeight) (int)SendMessage((hGrid),BGM_SETTITLEHEIGHT,iHeight,0L)
#define BabyGrid_SetUnprotectColor(hGrid, clrUProtect) (int)SendMessage((hGrid),BGM_SETUNPROTECTCOLOR,(UINT) clrUProtect,0L)

#define BabyGrid_ShowHilight(hGrid,fShow) (int)SendMessage((hGrid),BGM_SHOWHILIGHT,fShow,0L)
#define BabyGrid_ShowIntegralRows(hGrid,fShow) (int)SendMessage((hGrid),BGM_SHOWINTEGRALROWS,fShow,0L)

//worker macro to simplify the main program code for loading data
#define BabyGrid_PutCell(hGrid,iRow,iCol,lpzText) { \
	BGcell.row = iRow; BGcell.col = iCol; \
	BabyGrid_SetCellData(hGrid,&BGcell,lpzText); Sleep(1);}

//worker macro to simplify the main program code for retrieving data
#define BabyGrid_ReadCell(hGrid,iRow,iCol,lpzText) { \
	BGcell.row = iRow; BGcell.col = iCol; BabyGrid_GetCellData(hGrid,&BGcell,lpzText); }

//worker macro to simplify the main program code for deleting cells
#define BabyGrid_PopCell(hGrid,iRow,iCol) { \
	BGcell.row = iRow; BGcell.col = iCol; BabyGrid_DeleteCell(hGrid,&BGcell); }

//worker macros to simplify the main program code for cell attributes
#define BabyGrid_CellIsProtected(hGrid,iRow,iCol,fProtect) { \
	BGcell.row = iRow; BGcell.col = iCol; \
	fProtect = BabyGrid_GetProtection(hGrid,&BGcell); }

#define BabyGrid_SetCellProtection(hGrid,iRow,iCol,fSet) { \
	BGcell.row = iRow; BGcell.col = iCol; BabyGrid_ProtectCell(hGrid,&BGcell,fSet); }

#define BabyGrid_GetCellDataType(hGrid,iRow,iCol,iType) { \
	BGcell.row = iRow; BGcell.col = iCol; \
	iType = BabyGrid_GetType(hGrid,&BGcell); }

//To reduce flicker use the following instead of MoveWindow(...,bRepaint TRUE)
#define BabyGrid_Refresh(hWnd) RedrawWindow(hWnd,NULL,NULL,RDW_NOERASE|RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_UPDATENOW);

//Standard Window macro's for BabyGrid
#define BabyGrid_Enable(hwndCtl,fEnable) EnableWindow((hwndCtl),(fEnable))
#define BabyGrid_GetText(hwndCtl,lpch,cchMax) GetWindowText((hwndCtl),(lpch),(cchMax))
#define BabyGrid_GetTextLength(hwndCtl) GetWindowTextLength(hwndCtl)
#define BabyGrid_SetText(hwndCtl,lpsz) SetWindowText((hwndCtl),(lpsz))



#endif