#include "TabTracerLog.h"

static HFONT hFont;

LRESULT CALLBACK WindowProcTabTracerLog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	HDC memHdc = NULL;
	POINT mousepos;
	HBITMAP bufBMP;

	switch (uMsg)
	{
	case WM_CREATE:
		
		/* Initiele default waarden (voor welke niet met settings worden geladen) */
		tracerlayout.rs_height = 3;
		tracerlayout.pad_left = 10;
		tracerlayout.pad_right = 130;
		tracerlayout.pad_bottom = 5;
		tracerlayout.text_top = 5 + iTracerTop;

		/* Aanmakaen 'zoom switch' knop */
		hLogZoomswitch = CreateWindowEx(
			0, "BUTTON", "S/TS", WS_CHILD,
			325, iTracerTop - 33, 40, 21,
			hWnd, NULL, hMainInstance, NULL);
		/* Aanmakaen 'zoom in' knop */
		hLogZoomin = CreateWindowEx(
			0, "BUTTON", "+", WS_CHILD,
			375, iTracerTop - 33, 40, 21,
			hWnd, NULL, hMainInstance, NULL);
		/* Aanmakaen 'zoom out' knop */
		hLogZoomout = CreateWindowEx(
			0, "BUTTON", "-", WS_CHILD,
			425, iTracerTop - 33, 40, 21,
			hWnd, NULL, hMainInstance, NULL);

		InvalidateRect(hWnd, NULL, TRUE);

		// Set tab text font
		hFont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");

		break;

	case WM_DESTROY:
		break;

	case WM_LBUTTONDOWN:
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		GetCursorPos(&mousepos);
		ScreenToClient(hWnd, &mousepos);
		//strTFBContent.iShowFasestat = (mousepos.y - 20) / 20;
		//if (mousepos.y < 20 || strTFBContent.iShowFasestat >= FC_MAX)strTFBContent.iShowFasestat = -1;
		break;

	case WM_PAINT:
	{
		GetClientRect(hWnd, &rect);

		hdc = GetDC(hWnd);
		memHdc = CreateCompatibleDC(hdc);
		bufBMP = CreateCompatibleBitmap(hdc, iTracerRight - iTracerLeft, iTracerBottom - iTracerTop);
		SelectObject(memHdc, bufBMP);

		BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, iTracerRight - iTracerLeft, iTracerBottom - iTracerTop, memHdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);

		iTracerTop = rect.top;
		iTracerLeft = rect.left;
		iTracerRight = rect.right;
		iTracerBottom = rect.bottom;

		break;
	}
	case WM_NOTIFY:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabTracerLogUpdate()
{
	int n, o, p, q;
	int trindexprev = ((trindex - 1) + TRACERBUFFERSIZE) % TRACERBUFFERSIZE;

	p = trindex;

	if (TE)
	{
		tracer_bloklog[trindex] = ML;
		for (n = 0; n < TRACES_MAX; ++n)
		{
			if (tracer[n])
			{
				q = ((trindex + TRACERBUFFERSIZE) % TRACERBUFFERSIZE);
				for (p = trindex; p < q; ++p)
				{
					switch (tracer[n]->type)
					{
					case Fase:
						tracer[n]->log[p][FASESTATUS] = (char)CG[tracer[n]->elem];
						//if (tfbmonitor[tracer[n]->elem].overstaan && CG[tracer[n]->elem] == _CG_RV)
						//	tracer[n]->log[p][FASESTATUS] = (char)CG_RV_OS;
						if (AR[tracer[n]->elem]) tracer[n]->log[p][REALISATIESTATUS] = 3;
						else if (G[tracer[n]->elem]) tracer[n]->log[p][REALISATIESTATUS] = 2;
						else if (A[tracer[n]->elem]) tracer[n]->log[p][REALISATIESTATUS] = 1;
						else tracer[n]->log[p][REALISATIESTATUS] = 0;
						break;
					case Detector:
						tracer[n]->log[p][0] = (char)D[tracer[n]->elem];
						tracer[n]->log[p][1] = (char)TDH[tracer[n]->elem];
						break;
					case Hulpelement:
						tracer[n]->log[p][0] = (char)H[tracer[n]->elem];
						if (EH[tracer[n]->elem]) tracer[n]->log[p][1] = 3;
						else if (SH[tracer[n]->elem]) tracer[n]->log[p][1] = 2;
						else if (IH[tracer[n]->elem]) tracer[n]->log[p][1] = 1;
						else tracer[n]->log[p][1] = 0;
						break;
					case Ingang:
						tracer[n]->log[p][0] = (char)IS[tracer[n]->elem];
						break;
					case Uitgang:
						tracer[n]->log[p][0] = (char)US[tracer[n]->elem];
						tracer[n]->log[p][1] = (char)GUS[tracer[n]->elem];
						break;
					case Counter:
						tracer[n]->log[p][0] = (char)C_counter[tracer[n]->elem];
						tracer[n]->log[p][1] = C_counter[tracer[n]->elem] == 0 ? 0 : tracer[n]->log[p][1];
						tracer[n]->log[p][1] = C_counter[tracer[n]->elem] > tracer[n]->log[trindexprev][0] ? 2 : tracer[n]->log[p][1];
						tracer[n]->log[p][1] = C_counter[tracer[n]->elem] < tracer[n]->log[trindexprev][0] ? 1 : tracer[n]->log[p][1];
						break;
					case Timer:
						tracer[n]->log[p][0] = (char)T[tracer[n]->elem];
						tracer[n]->log[p][1] = 0;
						if (ET[tracer[n]->elem]) tracer[n]->log[p][1] = 4;
						if (HT[tracer[n]->elem]) tracer[n]->log[p][1] = 3;
						if (RT[tracer[n]->elem]) tracer[n]->log[p][1] = 2;
						if (IT[tracer[n]->elem]) tracer[n]->log[p][1] = 1;
						break;
					case Memoryelem:
						tracer[n]->log[p][0] = (char)MM[tracer[n]->elem];
						tracer[n]->log[p][1] = 0;
						break;
					default:
						break;
					}
				}
			}
		}
		trindex += 1;
		if (trindex >= TRACERBUFFERSIZE) trindex = 0;
	}
}

void TabTracerLogReset()
{
}