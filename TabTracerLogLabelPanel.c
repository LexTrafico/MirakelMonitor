#include "TabTracerLogLabelPanel.h"

static int iTracerLabelLeft, iTracerLabelRight, iTracerLabelTop, iTracerLabelBottom;
static TRACERLAYOUT * tracerlayout;
static TRACERSTRUCT ** tracer;
static HFONT hLabelFont;

extern int tracer_count;

/* Variabelen voor scoll functies */
static int iVscrollPos;
static int iVscrollRange;
static int iVscrollMax;
static int iVscrollMin;
static int iHscrollPos;
static int iHscrollRange;
static int iHscrollMax;
static int iHscrollMin;

void TabTracerLogLabelPanel_drawLabels(HDC hDC, int type, int elem, int fc_height)
{
	/* Weergeven labels */
	int maxWidth = 25;
	SetBkMode(hDC, TRANSPARENT);
	switch (type)
	{
	case Fase:
		TextOut(hDC, 0, fc_height / 5 - 1, FC_code[elem], min(maxWidth, strlen(FC_code[elem])));
		break;
	case Detector:
		TextOut(hDC, 0, fc_height / 5 - 1, D_code[elem], min(maxWidth, strlen(D_code[elem])));
		break;
	case Counter:
		TextOut(hDC, 0, fc_height / 5 - 1, C_code[elem], min(maxWidth, strlen(C_code[elem])));
		break;
	case Timer:
		TextOut(hDC, 0, fc_height / 5 - 1, T_code[elem], min(maxWidth, strlen(T_code[elem])));
		break;
	case Hulpelement:
		TextOut(hDC, 0, fc_height / 5 - 1, H_code[elem], min(maxWidth, strlen(H_code[elem])));
		break;
	case Ingang:
		TextOut(hDC, 0, fc_height / 5 - 1, IS_code[elem], min(maxWidth, strlen(IS_code[elem])));
		break;
	case Uitgang:
		TextOut(hDC, 0, fc_height / 5 - 1, US_code[elem], min(maxWidth, strlen(US_code[elem])));
		break;
	case Memoryelem:
		TextOut(hDC, 0, fc_height / 5 - 1, MM_code[elem], min(maxWidth, strlen(MM_code[elem])));
		break;
	default:
		break;
	}
}

HWND CreateTabTracerLogLabelPanel(HWND hwnd, HINSTANCE hInstance, RECT * r, TRACERLAYOUT * tracerl, TRACERSTRUCT ** tr)
{
	tracer = tr;
	tracerlayout = tracerl;
	RegisterSomeClass(hInstance, "TabTracerLogLabelPanelClass", (WNDPROC)WindowProcTabTracerLogLabelPanel);
	int e = GetLastError();
	HWND hWnd = CreateWindow(
		"TabTracerLoglLabelPanelClass", "",
		WS_CHILD,
		r->left, r->top, r->right - r->left, r->bottom - r->top,
		hwnd, NULL, hInstance, NULL);
	e = GetLastError();
	iTracerLabelLeft = r->top;
	iTracerLabelLeft = r->left;
	iTracerLabelRight = r->right;
	iTracerLabelBottom = r->bottom;
	return hWnd;
}

LRESULT CALLBACK WindowProcTabTracerLogLabelPanel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_CREATE:
		hLabelFont = CreateFont(13, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Arial");
		return 0;

	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);

		iTracerLabelLeft = rect.top;
		iTracerLabelLeft = rect.left;
		iTracerLabelRight = rect.right;
		iTracerLabelBottom = rect.bottom;

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		/* Nieuwe verticale scroll info */
		SetScrollRange(hWnd, SB_VERT, 0, max(0, (tracer_count + 4 - height / tracerlayout->fc_height)), TRUE);
		if ((height / tracerlayout->fc_height) > tracer_count) iVscrollPos = 0;
		iVscrollPos = min(iVscrollPos, iVscrollMax);
		if (iVscrollPos != GetScrollPos(hWnd, SB_VERT))
		{
			SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);
		}
		///??? if (iTracerLabelRight - iTracerLabelLeft > (TRACERBUFFERSIZE / tracerzoom) * tracerlayout->timestep)
		///??? 	tracerlayout->timestep = (iTracerLabelRight - iTracerLabelLeft) / (TRACERBUFFERSIZE / tracerzoom);

		/* Nieuwe horizontale scroll info */
		//	int iHscrollPosTRA = iHscrollMax; // tijdelijke opslag van positie
		//	iHscrollMax = ((TRACERBUFFERSIZE / tracerzoom) - (int)((float)(iTracerLabelRight - iTracerLabelLeft) / (float)tracerlayout->timestep));
		//	SetScrollRange(hWnd, SB_HORZ, 0, iHscrollMax, TRUE);
		//	iHscrollPos = iHscrollMax - iHscrollPosTRA + iHscrollPos; // terugzetten positie
		//	if (iHscrollPos != GetScrollPos(hWnd, SB_HORZ))
		//	{
		//		SetScrollPos(hWnd, SB_HORZ, iHscrollPos, TRUE);
		//	}
		//	iHscrollPosTRA = iHscrollPos < 0 ? iHscrollMax : iHscrollPos;

		return 0;
	case WM_PAINT:

		hdc = GetDC(hWnd);
		BeginPaint(hWnd, &ps);
		for (int tr = 0; tr < TRACES_MAX; ++tr)
		{
			if (tracer[tr])
			{
				TabTracerLogLabelPanel_drawLabels(hdc, tracer[tr]->type, tracer[tr]->elem, tracerlayout->fc_height);	
			}
		}
		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}