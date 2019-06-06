#include "MirakelMonitorUtilities.h"

HBRUSH hBrushMirakel[BRUSH_MAX];
HPEN hPenMirakel[BRUSH_MAX];
HBRUSH hBrushCG[_CG_MAX + 1];

COLORREF greenprtext = RGB(20, 155, 20);
COLORREF greenartext = RGB(20, 255, 20);
COLORREF redtext = RGB(155, 20, 20);
COLORREF whitetext = RGB(255, 255, 255);
COLORREF blacktext = RGB(0, 0, 0);

void Initialize(HWND hWnd)
{
	TEXTMETRIC tm;
	HDC hDC = GetDC(hWnd);
	GetTextMetrics(hDC, &tm);
	iCharWidth = tm.tmAveCharWidth;
	iCharHeight = tm.tmHeight + tm.tmExternalLeading;
}

void CreateBrushes()
{
	hBrushMirakel[BRUSH_RED] = CreateSolidBrush(RGB(255, 0, 0));
	hBrushMirakel[BRUSH_DARKRED] = CreateSolidBrush(RGB(128, 0, 0));
	hBrushMirakel[BRUSH_GREEN] = CreateSolidBrush(RGB(0, 255, 0));
	hBrushMirakel[BRUSH_DARKGREEN] = CreateSolidBrush(RGB(0, 128, 0));
	hBrushMirakel[BRUSH_BLUE] = CreateSolidBrush(RGB(0, 0, 255));
	hBrushMirakel[BRUSH_DARKBLUE] = CreateSolidBrush(RGB(0, 0, 128));
	hBrushMirakel[BRUSH_YELLOW] = CreateSolidBrush(RGB(255, 255, 0));
	hBrushMirakel[BRUSH_ORANGE] = CreateSolidBrush(RGB(255, 128, 0));
	hBrushMirakel[BRUSH_MAGENTA] = CreateSolidBrush(RGB(255, 128, 0));
	hBrushMirakel[BRUSH_TEAL] = CreateSolidBrush(RGB(0, 128, 128));
	hBrushMirakel[BRUSH_WHITE] = CreateSolidBrush(RGB(255, 255, 255));
	hBrushMirakel[BRUSH_BLACK] = CreateSolidBrush(RGB(0, 0, 0));
	hBrushMirakel[BRUSH_GRAY] = CreateSolidBrush(RGB(128, 128, 128));
	hBrushMirakel[BRUSH_LIGHTGRAY] = CreateSolidBrush(RGB(192, 192, 192));
	hBrushMirakel[BRUSH_DARKGRAY] = CreateSolidBrush(RGB(64, 64, 64));
	
	hBrushCG[_CG_RA] = CreateSolidBrush(RGB(255, 0, 0));      /* RA */
	hBrushCG[_CG_VS] = CreateSolidBrush(RGB(0, 0, 255));      /* VS */
	hBrushCG[_CG_FG] = CreateSolidBrush(RGB(0, 128, 0));      /* FG */
	hBrushCG[_CG_WG] = CreateSolidBrush(RGB(0, 128, 128));    /* WG */
	hBrushCG[_CG_VG] = CreateSolidBrush(RGB(0, 255, 0));      /* VG */
	hBrushCG[_CG_MG] = CreateSolidBrush(RGB(255, 255, 255));  /* MG */
	hBrushCG[_CG_GL] = CreateSolidBrush(RGB(255, 255, 0));    /* GL */
	hBrushCG[_CG_RV] = CreateSolidBrush(RGB(128, 0, 0));      /* R  */
	hBrushCG[_CG_NO] = CreateSolidBrush(RGB(160, 160, 160));  /* none */
	hBrushCG[_CG_BG] = CreateSolidBrush(RGB(221, 221, 221));  /* background */
	hBrushCG[_CG_MAX] = CreateSolidBrush(RGB(221, 221, 221)); /* GR */

	hPenMirakel[BRUSH_RED] = CreatePen(0, 1, RGB(255, 0, 0));
	hPenMirakel[BRUSH_DARKRED] = CreatePen(0, 1, RGB(128, 0, 0));
	hPenMirakel[BRUSH_GREEN] = CreatePen(0, 1, RGB(0, 255, 0));
	hPenMirakel[BRUSH_DARKGREEN] = CreatePen(0, 1, RGB(0, 128, 0));
	hPenMirakel[BRUSH_BLUE] = CreatePen(0, 1, RGB(0, 0, 255));
	hPenMirakel[BRUSH_DARKBLUE] = CreatePen(0, 1, RGB(0, 0, 128));
	hPenMirakel[BRUSH_YELLOW] = CreatePen(0, 1, RGB(255, 255, 0));
	hPenMirakel[BRUSH_ORANGE] = CreatePen(0, 1, RGB(255, 128, 0));
	hPenMirakel[BRUSH_MAGENTA] = CreatePen(0, 1, RGB(255, 128, 0));
	hPenMirakel[BRUSH_TEAL] = CreatePen(0, 1, RGB(0, 128, 128));
	hPenMirakel[BRUSH_WHITE] = CreatePen(0, 1, RGB(255, 255, 255));
	hPenMirakel[BRUSH_BLACK] = CreatePen(0, 1, RGB(0, 0, 0));
	hPenMirakel[BRUSH_GRAY] = CreatePen(0, 1, RGB(128, 128, 128));
	hPenMirakel[BRUSH_LIGHTGRAY] = CreatePen(0, 1, RGB(192, 192, 192));
	hPenMirakel[BRUSH_DARKGRAY] = CreatePen(0, 1, RGB(64, 64, 64));
}

ATOM RegisterSomeClass(HINSTANCE hInstance, char * className, WNDPROC proc)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;

	return RegisterClass(&wc);
}

HWND CreateTabDisplayWindow(HWND hwndTab, HINSTANCE hInstance, char * pcClassName, WNDPROC wProc)
{
	RECT r;
	RegisterSomeClass(hInstance, pcClassName, (WNDPROC)wProc);
	int e = GetLastError();
	GetClientRect(hwndTab, &r);
	TabCtrl_AdjustRect(hwndTab, 0, &r);
	HWND hWnd = CreateWindow(
		pcClassName, "",
		WS_CHILD | WS_VISIBLE,
		r.left, r.top, r.right - r.left, r.bottom - r.top,
		hwndTab, NULL, hInstance, NULL);
	e = GetLastError();
	return hWnd;
}

/* Function to add cstring items to a treeview control */
HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, HTREEITEM hParent, int num)
{
	TVITEM tvi;
	TVINSERTSTRUCT  tvins;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	static HTREEITEM hPrevRootItem = NULL;
	static HTREEITEM hPrevLev2Item = NULL;
	HTREEITEM hti;

	memset(&tvins, 0, sizeof(tvins));
	memset(&tvi, 0, sizeof(tvi));

	tvi.mask = TVIF_TEXT | TVIF_PARAM;

	// Set the text of the item. 
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Save the heading level in the item's application-defined 
	// data area. 
	tvi.lParam = (LPARAM)num;
	tvins.item = tvi;
	tvins.hInsertAfter = hPrev;

	// Set the parent item based on the specified level. 
	if (hParent == NULL) tvins.hParent = TVI_ROOT;
	else tvins.hParent = hParent;

	// Add the item to the tree-view control. 
	hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM,
		0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (hPrev == NULL)
		return NULL;

	return hPrev;
}
