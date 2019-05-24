#include "TabFasenlog.h"

LRESULT CALLBACK WindowProcTabFasenlog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	HDC memHdc = NULL;

	switch (uMsg)
	{
	case WM_PAINT:
	{
		GetClientRect(hWnd, &rect);
		BeginPaint(hWnd, &ps);
		SelectObject(ps.hdc, hBrushMirakel[BRUSH_GREEN]);
		Rectangle(ps.hdc, 10, 10, rect.right - 10, rect.bottom - 10);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		break;
	}
	case WM_NOTIFY:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}