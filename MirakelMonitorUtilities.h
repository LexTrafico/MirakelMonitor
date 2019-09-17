#pragma once

#include <Windows.h>
#include "Windowsx.h"
#include <CommCtrl.h>
#include "CCOLDefines.h"
#include "DataGridView.h"

typedef enum MirakelColorsEnum
{
	BRUSH_RED,
	BRUSH_DARKRED,
	BRUSH_GREEN,
	BRUSH_DARKGREEN,
	BRUSH_BLUE,
	BRUSH_DARKBLUE,
	BRUSH_YELLOW,
	BRUSH_ORANGE,
	BRUSH_MAGENTA,
	BRUSH_TEAL,
	BRUSH_WHITE,
	BRUSH_BLACK,
	BRUSH_GRAY,
	BRUSH_LIGHTGRAY,
	BRUSH_LIGHTBLUE,
	BRUSH_DARKGRAY,
	BRUSH_NULL,
	BRUSH_MAX
};

extern HBRUSH hBrushMirakel[BRUSH_MAX];
extern HBRUSH hBrushCG[_CG_MAX + 1];
extern HPEN hPenMirakel[BRUSH_MAX];
extern HPEN hPenCG[_CG_MAX + 1];
extern HWND hMainTab;
extern HINSTANCE hMainInstance;

extern int iCharWidth, iCharHeight;

extern COLORREF greenprtext;
extern COLORREF greenartext;
extern COLORREF redtext;
extern COLORREF whitetext;
extern COLORREF blacktext;

#define SZBUFFERSIZE 128

extern char lpszTemp1[SZBUFFERSIZE];

void Initialize(HWND hWnd);
void CreateBrushes();
HWND CreateTabDisplayWindow(HWND hwndTab, HINSTANCE hInstance, char * pcClassName, WNDPROC wProc);
ATOM RegisterSomeClass(HINSTANCE hInstance, char * className, WNDPROC proc);
HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, HTREEITEM hParent, int num);

#define MIRMSG_TABCHANGE WM_USER + 0x13
