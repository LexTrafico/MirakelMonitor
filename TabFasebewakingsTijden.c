#include "TabFasebewakingsTijden.h"

static HFONT hFont;

LRESULT CALLBACK WindowProcTabWachttijden(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	HDC memHdc = NULL;
	POINT mousepos;

	switch (uMsg)
	{
	case WM_CREATE:
		pstrTFBMonitor = (TFBMONSTRUCT *)malloc(FC_MAX * sizeof(TFBMONSTRUCT));
		strTFBContent.hwndTip = (HWND *)malloc(FC_MAX * sizeof(HWND));
		strTFBContent.ti = (TOOLINFO *)malloc(FC_MAX * sizeof(TOOLINFO));
		TabWachttijdenReset();
		strTFBContent.iShowFasestat = -1;

		// Set tab text font
		hFont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		
		/* Aanmaken en toewijzen tooltip per fase in de monitor */
		for (int fc = 0; fc < FC_MAX; ++fc)
		{
			/* Init toolinfo struct */
			strTFBContent.ti[fc].cbSize = sizeof(TOOLINFO);
			strTFBContent.ti[fc].uFlags = TTF_SUBCLASS;
			strTFBContent.ti[fc].hwnd = hWnd;
			strTFBContent.ti[fc].hinst = hMainInstance;

			/* registreren tooltip window */
			strTFBContent.hwndTip[fc] = CreateWindowEx(
				WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
				WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				hWnd, NULL, hMainInstance, NULL);
			/* Set window positie */
			SetWindowPos(strTFBContent.hwndTip[fc], HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			/* Instellen initiele tekst weergave */
			sprintf_s(lpszTemp1, SZBUFFERSIZE, "FC%s: geen data", FC_code[fc]);
			/* Instellen tooltip parameters: tekst en bijbehorende rectangle */
			strTFBContent.ti[fc].lpszText = lpszTemp1;
			strTFBContent.ti[fc].rect.left = fc * 20 + iCharWidth * 3 + iTFBLeft;
			strTFBContent.ti[fc].rect.right = fc * 20 + iCharWidth * 3 + 20 + iTFBLeft;
			strTFBContent.ti[fc].rect.top = iTFBTop;
			strTFBContent.ti[fc].rect.bottom = 1000;

			/* ToolTip associeren met fc "window" (=rect). */
			SendMessage(strTFBContent.hwndTip[fc], TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&strTFBContent.ti[fc]);
		}
		break;

	case WM_DESTROY:
		free(pstrTFBMonitor);
		free(strTFBContent.hwndTip);
		free(strTFBContent.ti);
		break;

	case WM_LBUTTONDOWN:
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		GetCursorPos(&mousepos);
		ScreenToClient(hWnd, &mousepos);
		strTFBContent.iShowFasestat = (mousepos.x - iTFBLeft - iCharHeight * 3 + 30) / 20;
		strTFBContent.iShowFasestat = strTFBContent.iShowFasestat >= FC_MAX ? FC_MAX - 1 : strTFBContent.iShowFasestat;
		status_phasecycles(strTFBContent.lpszFasestat, strTFBContent.iShowFasestat, 0);
		break;

	case WM_PAINT:
	{
		GetClientRect(hWnd, &rect);
		BeginPaint(hWnd, &ps);
		
		int fcl;
		if (pstrTFBMonitor)
		{
			/* Weergeven schaal indicatie */
			SetTextColor(ps.hdc, RGB(0, 0, 0));
			SetBkMode(ps.hdc, TRANSPARENT);
			SelectObject(ps.hdc, hFont);
			TextOut(ps.hdc, iTFBLeft, iTFBBottom - iCharHeight * 3 - 10, "  0", 3);
			TextOut(ps.hdc, iTFBLeft, iTFBBottom - iCharHeight * 3 - 110, "100", 3);
			TextOut(ps.hdc, iTFBLeft, iTFBBottom - iCharHeight * 3 - 210, "200", 3);
			TextOut(ps.hdc, iTFBLeft, iTFBBottom - iCharHeight * 3 - 310, "300", 3);
			SetBkMode(ps.hdc, OPAQUE);
			SelectObject(ps.hdc, hPenMirakel[BRUSH_LIGHTGRAY]);
			MoveToEx(ps.hdc, iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3, NULL);
			LineTo(ps.hdc, 20 * FC_MAX + iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3);
			MoveToEx(ps.hdc, iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3 - 100, NULL);
			LineTo(ps.hdc, 20 * FC_MAX + iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3 - 100);
			MoveToEx(ps.hdc, iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3 - 200, NULL);
			LineTo(ps.hdc, 20 * FC_MAX + iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3 - 200);
			MoveToEx(ps.hdc, iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3 - 300, NULL);
			LineTo(ps.hdc, 20 * FC_MAX + iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3 - 300);
			SelectObject(ps.hdc, hPenMirakel[BRUSH_BLACK]);

			if (strTFBContent.iShowFasestat != -1)
			{
				if (strTFBContent.iShowFasestat < 0) strTFBContent.iShowFasestat = 0;
				if (strTFBContent.iShowFasestat >= FC_MAX) strTFBContent.iShowFasestat = FC_MAX - 1;
				status_phasecycles(strTFBContent.lpszFasestat, strTFBContent.iShowFasestat, 0);
				SetBkMode(ps.hdc, TRANSPARENT);
				TextOut(ps.hdc, iTFBLeft, iTFBBottom - iCharHeight, strTFBContent.lpszFasestat, strlen(strTFBContent.lpszFasestat));
				SetBkMode(ps.hdc, OPAQUE);
			}

			for (fcl = 0; fcl < FC_MAX; ++fcl)
			{
				/* Kleur voor lopende tijdbalk */
				SelectObject(ps.hdc, hBrushMirakel[BRUSH_GREEN]);
				/* Omlijning van balk: rood in geval van bOverstaan */
				if (pstrTFBMonitor[fcl].bOverstaan)
					SelectObject(ps.hdc, hPenMirakel[BRUSH_RED]);
				else
					SelectObject(ps.hdc, hPenMirakel[BRUSH_GRAY]);
				/* Lopende tfb weergeven */
				Rectangle(ps.hdc,
					fcl * 20 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 3 - pstrTFBMonitor[fcl].iTFBTimer,
					fcl * 20 + 15 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 3);
				/* Maximale waarde */
				SelectObject(ps.hdc, hPenMirakel[BRUSH_RED]);
				Rectangle(ps.hdc,
					fcl * 20 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 3 - pstrTFBMonitor[fcl].iTFBMax - 1,
					fcl * 20 + 15 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 3 - pstrTFBMonitor[fcl].iTFBMax + 1);
				/* Gemiddelde waarde */
				SelectObject(ps.hdc, hPenMirakel[BRUSH_BLUE]);
				Rectangle(ps.hdc,
					fcl * 20 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 3 - pstrTFBMonitor[fcl].iTFBGemiddeld - 1,
					fcl * 20 + 15 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 3 - pstrTFBMonitor[fcl].iTFBGemiddeld + 1);
				SelectObject(ps.hdc, hPenMirakel[BRUSH_GREEN]);
				if (G[fcl] && PR[fcl])
					SetTextColor(ps.hdc, greenprtext);
				else if (G[fcl] && AR[fcl])
					SetTextColor(ps.hdc, greenartext);
				else
					SetTextColor(ps.hdc, redtext);
				SetBkMode(ps.hdc, TRANSPARENT);
				TextOut(ps.hdc, fcl * 20 + iCharWidth * 4 + iTFBLeft, iTFBBottom - iCharHeight * 3, FC_code[fcl], strlen(FC_code[fcl]));
				SetBkMode(ps.hdc, OPAQUE);

				/* FC Status */
				SelectObject(ps.hdc, hPenMirakel[BRUSH_GRAY]);
				SelectObject(ps.hdc, hBrushCG[CG[fcl]]);
				Rectangle(ps.hdc,
					fcl * 20 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 1 - 15,
					fcl * 20 + 15 + iCharWidth * 4 + iTFBLeft,
					iTFBBottom - iCharHeight * 1);
			}
			SetTextColor(ps.hdc, blacktext);
		}

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
		
		iTFBRight = width;
		iTFBBottom = height - 10;
		
		break;
	}
	case WM_NOTIFY:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabWachttijdenUpdate()
{
	if (pstrTFBMonitor)
	{
		for (int fc = 0; fc < FC_MAX; ++fc)
		{
			if (R[fc] && A[fc])
			{
				pstrTFBMonitor[fc].iTFBTimer = TFB_timer[fc];
				if (pstrTFBMonitor[fc].iTFBTimer > pstrTFBMonitor[fc].iTFBMax)
				{
					pstrTFBMonitor[fc].iTFBMax = pstrTFBMonitor[fc].iTFBTimer;
					sprintf_s(lpszTemp1, SZBUFFERSIZE, "FC%s - Max: %d (%2.2d-%2.2d-%4d %2.2d:%2.2d:%2.2d) Gem.: %d",
						FC_code[fc], pstrTFBMonitor[fc].iTFBMax,
						CIF_KLOK[_CIF_DAG], CIF_KLOK[_CIF_MAAND], CIF_KLOK[_CIF_JAAR],
						CIF_KLOK[_CIF_UUR], CIF_KLOK[_CIF_MINUUT], CIF_KLOK[_CIF_SECONDE],
						pstrTFBMonitor[fc].iTFBGemiddeld);
					strTFBContent.ti[fc].lpszText = lpszTemp1;
					SendMessage(strTFBContent.hwndTip[fc], TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&strTFBContent.ti[fc]);
				}
			}
			if (SG[fc])
			{
				++pstrTFBMonitor[fc].iTFBCount;
				pstrTFBMonitor[fc].ulTFBTotal += pstrTFBMonitor[fc].iTFBTimer;
				pstrTFBMonitor[fc].iTFBGemiddeld = pstrTFBMonitor[fc].ulTFBTotal / pstrTFBMonitor[fc].iTFBCount;
				pstrTFBMonitor[fc].iTFBTimer = 0;
				
				sprintf_s(lpszTemp1, SZBUFFERSIZE, "FC%s: Laatste: %d (%2.2d-%2.2d-%4d %2.2d:%2.2d:%2.2d) Gem.: %d",
					FC_code[fc], pstrTFBMonitor[fc].iTFBMax,
					CIF_KLOK[_CIF_DAG], CIF_KLOK[_CIF_MAAND], CIF_KLOK[_CIF_JAAR],
					CIF_KLOK[_CIF_UUR], CIF_KLOK[_CIF_MINUUT], CIF_KLOK[_CIF_SECONDE],
					pstrTFBMonitor[fc].iTFBGemiddeld);
				strTFBContent.ti[fc].lpszText = lpszTemp1;
				SendMessage(strTFBContent.hwndTip[fc], TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&strTFBContent.ti[fc]);
			}
			if (EGL[fc] && PR[fc] && MK[fc])
				pstrTFBMonitor[fc].bOverstaan = TRUE;
			else if (EGL[fc] || SG[fc])
				pstrTFBMonitor[fc].bOverstaan = FALSE;
			iTFBHoogsteooit = pstrTFBMonitor[fc].iTFBMax > iTFBHoogsteooit ? pstrTFBMonitor[fc].iTFBMax : iTFBHoogsteooit;
		}
	}
}

void TabWachttijdenReset()
{
	if (pstrTFBMonitor)
	{
		for (int fc = 0; fc < FC_MAX; ++fc)
		{
			pstrTFBMonitor[fc].iTFBTimer = 0;
			pstrTFBMonitor[fc].iTFBCount = 0;
			pstrTFBMonitor[fc].iTFBGemiddeld = 0;
			pstrTFBMonitor[fc].iTFBMax = 0;
			pstrTFBMonitor[fc].ulTFBTotal = 0;
			pstrTFBMonitor[fc].bOverstaan = 0;
		}
		iTFBHoogsteooit = 0;
	}
}