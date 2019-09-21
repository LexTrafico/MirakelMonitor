#include "TabFasebewakingsTijden.h"

static HFONT hFont;

LRESULT CALLBACK WindowProcTabWachttijden(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	POINT mousepos;

	switch (uMsg)
	{
	case WM_CREATE:
		pstrTFBMonitor = (TFBMONSTRUCT *)malloc(FC_MAX * sizeof(TFBMONSTRUCT));
		for (int fc = 0; fc < FC_MAX; ++fc)
		{
			pstrTFBMonitor[fc].iTFBCurrent = 0;
			pstrTFBMonitor[fc].iTFBCount = 0;
			pstrTFBMonitor[fc].iTFBMeasured = (int *)malloc(TFBMAXMEASURE * sizeof(int));
		}
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
			strTFBContent.ti[fc].rect.left = iTFBLeft + 2;
			strTFBContent.ti[fc].rect.right = iTFBLeft + 17;
			strTFBContent.ti[fc].rect.top = iTFBTop + fc * 20 + 22;
			strTFBContent.ti[fc].rect.bottom = iTFBTop + fc * 20 + 37;

			/* ToolTip associeren met fc "window" (=rect). */
			SendMessage(strTFBContent.hwndTip[fc], TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&strTFBContent.ti[fc]);
		}
		return 0;

	case WM_DESTROY:
		for (int fc = 0; fc < FC_MAX; ++fc)
		{
			free(pstrTFBMonitor[fc].iTFBMeasured);
		}
		free(pstrTFBMonitor);
		free(strTFBContent.hwndTip);
		free(strTFBContent.ti);
		return 0;

	case WM_LBUTTONDOWN:
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		GetCursorPos(&mousepos);
		ScreenToClient(hWnd, &mousepos);
		strTFBContent.iShowFasestat = (mousepos.y - 20) / 20;
		if (mousepos.y < 20 || strTFBContent.iShowFasestat >= FC_MAX)strTFBContent.iShowFasestat = -1;
		return 0;

	case WM_PAINT:
	{
		GetClientRect(hWnd, &rect);

		hdc = GetDC(hWnd);
		
		int fcl;
		int leftSpace = 50;
		if (pstrTFBMonitor)
		{
			/* Weergeven schaal indicatie */
			SelectObject(hDCTabTFB, hBrushMirakel[BRUSH_LIGHTGRAY]);
			Rectangle(hDCTabTFB,
				iTFBLeft,
				iTFBTop,
				iTFBRight,
				iTFBBottom);
			SetTextColor(hDCTabTFB, RGB(0, 0, 0));
			SetBkMode(hDCTabTFB, TRANSPARENT);
			SelectObject(hDCTabTFB, hFont);
			TextOut(hDCTabTFB, leftSpace + iTFBLeft, iTFBTop, "0", 1);
			TextOut(hDCTabTFB, leftSpace + iTFBLeft + 100, iTFBTop, "100", 3);
			TextOut(hDCTabTFB, leftSpace + iTFBLeft + 200, iTFBTop, "200", 3);
			TextOut(hDCTabTFB, leftSpace + iTFBLeft + 300, iTFBTop, "300", 3);
			SetBkMode(hDCTabTFB, OPAQUE);
			SelectObject(hDCTabTFB, hPenMirakel[BRUSH_DARKGRAY]);
			MoveToEx(hDCTabTFB, leftSpace + iTFBLeft, iTFBTop + 20, NULL);
			LineTo(hDCTabTFB, leftSpace + iTFBLeft, iTFBTop + 20 + 20 * FC_MAX);
			MoveToEx(hDCTabTFB, leftSpace + iTFBLeft + 100, iTFBTop + 20, NULL);
			LineTo(hDCTabTFB, leftSpace + iTFBLeft + 100, iTFBTop + 20 + 20 * FC_MAX);
			MoveToEx(hDCTabTFB, leftSpace + iTFBLeft + 200, iTFBTop + 20, NULL);
			LineTo(hDCTabTFB, leftSpace + iTFBLeft + 200, iTFBTop + 20 + 20 * FC_MAX);
			MoveToEx(hDCTabTFB, leftSpace + iTFBLeft + 300, iTFBTop + 20, NULL);
			LineTo(hDCTabTFB, leftSpace + iTFBLeft + 300, iTFBTop + 20 + 20 * FC_MAX);

			for (fcl = 0; fcl < FC_MAX; ++fcl)
			{
				/* Kleur voor lopende tijdbalk */
				SelectObject(hDCTabTFB, hBrushMirakel[BRUSH_DARKRED]);
				/* Omlijning van balk: rood in geval van bOverstaan */
				if (pstrTFBMonitor[fcl].bOverstaan)
					SelectObject(hDCTabTFB, hPenMirakel[BRUSH_RED]);
				else
					SelectObject(hDCTabTFB, hPenMirakel[BRUSH_GRAY]);
				/* Lopende tfb weergeven */
				Rectangle(hDCTabTFB,
					iTFBLeft + leftSpace,
					iTFBTop + fcl * 20 + 20,
					iTFBLeft + leftSpace + pstrTFBMonitor[fcl].iTFBTimer,
					iTFBTop + fcl * 20 + 35);
				/* Maximale waarde */
				SelectObject(hDCTabTFB, hPenMirakel[BRUSH_RED]);
				Rectangle(hDCTabTFB,
					iTFBLeft + leftSpace + pstrTFBMonitor[fcl].iTFBMax,
					iTFBTop + fcl * 20 + 20,
					iTFBLeft + leftSpace + pstrTFBMonitor[fcl].iTFBMax + 2,
					iTFBTop + fcl * 20 + 35);
				/* Gemiddelde waarde */
				SelectObject(hDCTabTFB, hPenMirakel[BRUSH_BLUE]);
				Rectangle(hDCTabTFB,
					iTFBLeft + leftSpace + pstrTFBMonitor[fcl].iTFBGemiddeld,
					iTFBTop + fcl * 20 + 20,
					iTFBLeft + leftSpace + pstrTFBMonitor[fcl].iTFBGemiddeld + 2,
					iTFBTop + fcl * 20 + 35);
				SelectObject(hDCTabTFB, hPenMirakel[BRUSH_GREEN]);
				if (G[fcl] && PR[fcl])
					SetTextColor(hDCTabTFB, greenprtext);
				else if (G[fcl] && AR[fcl])
					SetTextColor(hDCTabTFB, greenartext);
				else
					SetTextColor(hDCTabTFB, redtext);
				SetBkMode(hDCTabTFB, TRANSPARENT);
				TextOut(hDCTabTFB, iTFBLeft + 19, fcl * 20 + 20 + iTFBTop, FC_code[fcl], strlen(FC_code[fcl]));
				SetBkMode(hDCTabTFB, OPAQUE);

				/* FC Status */
				SelectObject(hDCTabTFB, hPenMirakel[BRUSH_GRAY]);
				SelectObject(hDCTabTFB, hBrushCG[CG[fcl]]);
				Rectangle(hDCTabTFB,
					iTFBLeft + 2,
					iTFBTop + fcl * 20 + 22,
					iTFBLeft + 17,
					iTFBTop + fcl * 20 + 37);
			}

			if (strTFBContent.iShowFasestat != -1)
			{
				SelectObject(hDCTabTFB, hBrushMirakel[BRUSH_GRAY]);
				Rectangle(hDCTabTFB,
					iTFBLeft,
					iTFBBottom - iCharHeight - 4,
					iTFBRight,
					iTFBBottom);
				SetTextColor(hDCTabTFB, RGB(0, 0, 0));
				if (strTFBContent.iShowFasestat < 0) strTFBContent.iShowFasestat = 0;
				if (strTFBContent.iShowFasestat >= FC_MAX) strTFBContent.iShowFasestat = FC_MAX - 1;
				status_phasecycles(strTFBContent.lpszFasestat, strTFBContent.iShowFasestat, 0);
				SetBkMode(hDCTabTFB, TRANSPARENT);
				TextOut(hDCTabTFB, iTFBLeft + 4, iTFBBottom - iCharHeight - 2, strTFBContent.lpszFasestat, strlen(strTFBContent.lpszFasestat));
				SetBkMode(hDCTabTFB, OPAQUE);
			}

			SetTextColor(hDCTabTFB, blacktext);
		}

		BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, iTFBRight - iTFBLeft, iTFBBottom - iTFBTop, hDCTabTFB, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);
		return 0;
	}
	case WM_SIZE:
	{
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);

		iTFBTop = rect.top;
		iTFBLeft = rect.left;
		iTFBRight = rect.right;
		iTFBBottom = rect.bottom;

		hdc = GetDC(hWnd);
		hDCTabTFB = CreateCompatibleDC(hdc);
		hBitmapTFB = CreateCompatibleBitmap(hdc, iTFBRight - iTFBLeft, iTFBBottom - iTFBTop);
		SelectObject(hDCTabTFB, hBitmapTFB);
		ReleaseDC(hWnd, hdc);
		
		InvalidateRect(hWnd, &rect, TRUE);

		return 0;
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
				if (pstrTFBMonitor[fc].iTFBCount < TFBMAXMEASURE)
				{
					++pstrTFBMonitor[fc].iTFBCount;
				}
				pstrTFBMonitor[fc].iTFBMeasured[pstrTFBMonitor[fc].iTFBCurrent] = pstrTFBMonitor[fc].iTFBTimer;
				++pstrTFBMonitor[fc].iTFBCurrent;
				if (pstrTFBMonitor[fc].iTFBCurrent >= TFBMAXMEASURE) pstrTFBMonitor[fc].iTFBCurrent = 0;
				pstrTFBMonitor[fc].ulTFBTotal = 0;
				for (int i = 0; i < pstrTFBMonitor[fc].iTFBCount; ++i)
				{
					pstrTFBMonitor[fc].ulTFBTotal += pstrTFBMonitor[fc].iTFBMeasured[i];
				}
				int gemOld = pstrTFBMonitor[fc].iTFBGemiddeld;
				pstrTFBMonitor[fc].iTFBGemiddeld = (int)((double)pstrTFBMonitor[fc].ulTFBTotal / (double)pstrTFBMonitor[fc].iTFBCount);
				if (gemOld != pstrTFBMonitor[fc].iTFBGemiddeld)
				{
					sprintf_s(lpszTemp1, SZBUFFERSIZE, "FC%s - Max: %d (%2.2d-%2.2d-%4d %2.2d:%2.2d:%2.2d) Gem.: %d",
						FC_code[fc], pstrTFBMonitor[fc].iTFBMax,
						CIF_KLOK[_CIF_DAG], CIF_KLOK[_CIF_MAAND], CIF_KLOK[_CIF_JAAR],
						CIF_KLOK[_CIF_UUR], CIF_KLOK[_CIF_MINUUT], CIF_KLOK[_CIF_SECONDE],
						pstrTFBMonitor[fc].iTFBGemiddeld);
					strTFBContent.ti[fc].lpszText = lpszTemp1;
					SendMessage(strTFBContent.hwndTip[fc], TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&strTFBContent.ti[fc]);
				}
				pstrTFBMonitor[fc].iTFBTimer = 0;
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