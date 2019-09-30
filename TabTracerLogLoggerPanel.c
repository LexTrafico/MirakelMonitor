#include "TabTracerLogLoggerPanel.h"

static int iTracerLogLeft, iTracerLogRight, iTracerLogTop, iTracerLogBottom;
static TRACERLAYOUT * tracerlayout;
static TRACERSTRUCT ** tracer;
static HFONT hTimeLabelFont;
static int tracerzoom;
/* Variabelen voor scoll functies */
static int iVscrollPos;
static int iVscrollRange;
static int iVscrollMax;
static int iVscrollMin;
static int iHscrollPos;
static int iHscrollRange;
static int iHscrollMax;
static int iHscrollMin;

static int trlogtempsec, trlogtime;
static char trlogtimestamp[128];
static int tracer_id = 0;
static int tracer_count = 0;
extern int trindex;
static char tracer_bloklog[TRACERBUFFERSIZE];
static char tracerchanged = 0;

HWND CreateTabTracerLogLoggerPanel(HWND hwnd, HINSTANCE hInstance, RECT * r, TRACERLAYOUT * tracerl, TRACERSTRUCT ** tr)
{
	tracer = tr;
	tracerlayout = tracerl;
	RegisterSomeClass(hInstance, "TabTracerLogLoggerPanelClass", (WNDPROC)WindowProcTabTracerLogLoggerPanel);
	int e = GetLastError();
	HWND hWnd = CreateWindow(
		"TabTracerLogLoggerPanelClass", "",
		WS_CHILD,
		r->left, r->top, r->right - r->left, r->bottom - r->top,
		hwnd, NULL, hInstance, NULL);
	e = GetLastError();
	iTracerLogLeft = r->top;
	iTracerLogLeft = r->left;
	iTracerLogRight = r->right;
	iTracerLogBottom = r->bottom;
	return hWnd;
}

LRESULT CALLBACK WindowProcTabTracerLogLoggerPanel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hdc;
	HDC memHdc;
	HBITMAP bufBMP;
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_CREATE:
		hTimeLabelFont = CreateFont(13, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Arial");
		tracerzoom = 1;
		return 0;
	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);

		iTracerLogLeft = rect.top;
		iTracerLogLeft = rect.left;
		iTracerLogRight = rect.right;
		iTracerLogBottom = rect.bottom;
		
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
		if (iTracerLogRight - iTracerLogLeft > (TRACERBUFFERSIZE / tracerzoom) * tracerlayout->timestep)
			tracerlayout->timestep = (iTracerLogRight - iTracerLogLeft) / (TRACERBUFFERSIZE / tracerzoom);

		/* Nieuwe horizontale scroll info */
		int iHscrollPosTRA = iHscrollMax; // tijdelijke opslag van positie
		iHscrollMax = ((TRACERBUFFERSIZE / tracerzoom) - (int)((float)(iTracerLogRight - iTracerLogLeft) / (float)tracerlayout->timestep));
		SetScrollRange(hWnd, SB_HORZ, 0, iHscrollMax, TRUE);
		iHscrollPos = iHscrollMax - iHscrollPosTRA + iHscrollPos; // terugzetten positie
		if (iHscrollPos != GetScrollPos(hWnd, SB_HORZ))
		{
			SetScrollPos(hWnd, SB_HORZ, iHscrollPos, TRUE);
		}
		iHscrollPosTRA = iHscrollPos < 0 ? iHscrollMax : iHscrollPos;

		return 0;
	case WM_PAINT:
		
		if (tracerzoom == tracerzoom_SEC && !TS) break;
		
		GetClientRect(hWnd, &rect);

		hdc = GetDC(hWnd);
		memHdc = CreateCompatibleDC(hdc);
		bufBMP = CreateCompatibleBitmap(hdc, iTracerLogRight - iTracerLogLeft, iTracerLogBottom - iTracerLogTop);
		SelectObject(memHdc, bufBMP);

		int left = iTracerLogLeft;
		int top = iTracerLogTop;
		int right = iTracerLogRight;
		int bottom = iTracerLogBottom;

		SelectObject(memHdc, hPenMirakel[BRUSH_GRAY]);
		SelectObject(memHdc, hBrushMirakel[BRUSH_GRAY]);
		Rectangle(memHdc,
			0,
			top,
			iTracerLogRight,
			bottom);
		SelectObject(memHdc, hPenMirakel[BRUSH_LIGHTGRAY]);
		SelectObject(memHdc, hBrushMirakel[BRUSH_LIGHTGRAY]);
		Rectangle(memHdc,
			0,
			top,
			right,
			bottom);

		SelectObject(memHdc, GetStockObject(NULL_PEN));
		//SelectObject(memHdc, hItemLabelFont);

		for (int tr = 0; tr < TRACES_MAX; ++tr)
		{
			if (tracer[tr])
			{
				tabDebugTracer_paint_trace(memHdc, tr, left, top + tracer[tr]->place * tracerlayout->fc_height, right, bottom);
			}
		}
		/* Bovenkant van de log schoonvegen voor schrijven time stamps */
		SelectObject(memHdc, hBrushMirakel[BRUSH_LIGHTGRAY]);
		SelectObject(memHdc, hPenMirakel[BRUSH_LIGHTGRAY]);
		Rectangle(memHdc, left + 1, top, right - 1, top + 22);
		/* Laden juist text layout */
		SelectObject(memHdc, hTimeLabelFont);
		SelectObject(memHdc, hPenMirakel[BRUSH_LIGHTGRAY]);
		if (tracerzoom == tracerzoom_TSEC)
		{
			for (int j = ((iHscrollMax - iHscrollPos) * tracerzoom) * tracerlayout->timestep + right - ((CIF_KLOK[_CIF_TSEC_TELLER]) % tracerlayout->timeline) * tracerlayout->timestep;
				j > left + tracerlayout->pad_left;
				j -= tracerlayout->timestep * tracerlayout->timeline)
			{
				MoveToEx(memHdc, j, top, NULL);
				LineTo(memHdc, j, bottom);
				trlogtempsec = ((((iHscrollMax - iHscrollPos) * tracerzoom) * tracerlayout->timestep + right - j) / tracerlayout->timestep);
				trlogtime = ((CIF_KLOK[_CIF_UUR] * 3600) + (CIF_KLOK[_CIF_MINUUT] * 60) + CIF_KLOK[_CIF_SECONDE]) - (trlogtempsec / 10);
				trlogtempsec = CIF_KLOK[_CIF_SECONDE];
				sprintf_s(trlogtimestamp, SZBUFFERSIZE, "%2.2d:%2.2d:%2.2d", trlogtime / 3600, (trlogtime % 3600) / 60, (trlogtime % 3600) % 60);
				SetBkMode(memHdc, TRANSPARENT);
				TextOut(memHdc, j, top, trlogtimestamp, strlen(trlogtimestamp));
				SetBkMode(memHdc, OPAQUE);
			}
		}
		else if (tracerzoom == tracerzoom_SEC)
		{
			for (int j = ((((iHscrollMax - iHscrollPos) * tracerzoom) * tracerlayout->timestep) / 10) + right - tracerlayout->pad_right - (CIF_KLOK[_CIF_SECONDE] % tracerlayout->timeline) * tracerlayout->timestep;
				j > left + tracerlayout->pad_left;
				j -= tracerlayout->timestep * tracerlayout->timeline)
			{
				MoveToEx(memHdc, j, top + tracerlayout->pad_top, NULL);
				LineTo(memHdc, j, bottom);
				trlogtempsec = (((((iHscrollMax - iHscrollPos) * tracerzoom) * tracerlayout->timestep) / 10) + right - tracerlayout->pad_right - j) / tracerlayout->timestep;
				trlogtime = ((CIF_KLOK[_CIF_UUR] * 3600) + (CIF_KLOK[_CIF_MINUUT] * 60) + CIF_KLOK[_CIF_SECONDE]) - trlogtempsec;
				trlogtempsec = CIF_KLOK[_CIF_SECONDE];
				sprintf_s(trlogtimestamp, SZBUFFERSIZE, "%2.2d:%2.2d:%2.2d", trlogtime / 3600, (trlogtime % 3600) / 60, (trlogtime % 3600) % 60);
				SetBkMode(memHdc, TRANSPARENT);
				TextOut(memHdc, j, top, trlogtimestamp, strlen(trlogtimestamp));
				SetBkMode(memHdc, OPAQUE);
			}
		}

		BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, iTracerLogRight - iTracerLogLeft, iTracerLogBottom - iTracerLogTop, memHdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);
		return 0;
	}
}

void tabDebugTracer_paint_trace(HDC hDC, INT trace, INT left, INT top, INT right, INT bottom)
{
	int tmp_zoomofset = (trindex - ((iHscrollMax - iHscrollPos) * tracerzoom)) % tracerzoom;
	int i, ii, j;
	/* Opslaan virtuele verticale scroll positie */
	int log_pad_top_tmp = top + tracerlayout->pad_top - iVscrollPos * tracerlayout->fc_height;
	/* resert tijdelijke variabelen */
	int cg_stat_tmp = -1, cg_pos_tmp = -1;
	int rs_stat_tmp = -1, rs_pos_tmp = -1;
	for (ii = tracerzoom, i = trindex - tracerzoom - ((iHscrollMax - iHscrollPos) * tracerzoom), // -  tmp_zoomofset,			// i: toegang tot juiste plek in buffer: index minus huidige positie tov max
		j = right - tracerlayout->pad_right,					// j: plek op het scherm: start is rechts gecorrigeerd voor de padding
		cg_pos_tmp = j,
		rs_pos_tmp = j;			// tijdelijke positie variabelen gelijk aan startpositie
		j > left + tracerlayout->pad_left && (ii + tracerzoom) < TRACERBUFFERSIZE; ii += tracerzoom)// voorwaarde: plek op scherm pas nog binnen het tekenvlak voor de log
	{
		/* Index binnen max houden */
		if (i < 0) i = ((i + TRACERBUFFERSIZE) % TRACERBUFFERSIZE);
		/* Indien de status van de trace bij start niet overeen komt met de status op deze plek in de log:
			- opslaan nieuwe status
			- opslaan startplek van nieuwe status */
		if (cg_stat_tmp != tracer[trace]->log[i][0])
		{
			cg_stat_tmp = tracer[trace]->log[i][0];
			cg_pos_tmp = j;
		}
		if (rs_stat_tmp != tracer[trace]->log[i][1])
		{
			rs_stat_tmp = tracer[trace]->log[i][1];
			rs_pos_tmp = j;
		}
		i = (((i - tracerzoom) + TRACERBUFFERSIZE) % TRACERBUFFERSIZE);
		/* Zolang het past binnen het tekenvlak, en er zijn geen wijzigingen in de status, teruglezen in de log */
		while (((j -= tracerlayout->timestep)) > (left + tracerlayout->pad_left) &&
			tracer[trace]->log[i][0] == cg_stat_tmp && tracer[trace]->log[i][1] == rs_stat_tmp
			&& (ii + tracerzoom) < TRACERBUFFERSIZE)
		{
			i = (((i - tracerzoom) + TRACERBUFFERSIZE) % TRACERBUFFERSIZE);
			ii += tracerzoom;
		}
		/* Indien er een wijziging is in de status, schilderen van de status tot nu toe.
			Indien het niet meer past, schilderen restant */
		if ((tracer[trace]->log[i][0] != cg_stat_tmp) || (j - tracerlayout->timestep < tracerlayout->pad_left + left) || (ii + tracerzoom) >= TRACERBUFFERSIZE)
		{
			if (j - tracerlayout->timestep < tracerlayout->pad_left + left)
				j = tracerlayout->pad_left + left;

			switch (tracer[trace]->type)
			{
			case Fase:
				SelectObject(hDC, hBrushCG[cg_stat_tmp]);
				SelectObject(hDC, hPenCG[cg_stat_tmp]);
				if (cg_stat_tmp == _CG_RV)
					Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->rv_line_top, cg_pos_tmp, log_pad_top_tmp + tracerlayout->rv_line_bottom);
				//else if (cg_stat_tmp == CG_RV_OS)
				//	Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->rv_line_top - 1, cg_pos_tmp, log_pad_top_tmp + tracerlayout->rv_line_bottom);
				else
					Rectangle(hDC, j, log_pad_top_tmp, cg_pos_tmp, log_pad_top_tmp + tracerlayout->cg_height);
				break;
			case Hulpelement:
			case Counter:
			case Timer:
			case Detector:
			case Ingang:
			case Uitgang:
			case Memoryelem:
				if (cg_stat_tmp)
				{
					SelectObject(hDC, hBrushCG[_CG_VS]);
					SelectObject(hDC, hPenCG[_CG_VS]);
					Rectangle(hDC, j, log_pad_top_tmp, cg_pos_tmp, log_pad_top_tmp + tracerlayout->cg_height);
				}
				else
				{
					SelectObject(hDC, hBrushMirakel[BRUSH_BLACK]);
					SelectObject(hDC, hPenMirakel[BRUSH_BLACK]);
					Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->rv_line_top, cg_pos_tmp, log_pad_top_tmp + tracerlayout->rv_line_bottom);
				}
				break;
			default:
				break;
			}
		}
		/* Indien er een wijziging is in de realisatiestatus schilederen van de status tot nu toe.
			Indien het niet past, schilderen van restant */
		if ((tracer[trace]->log[i][1] != rs_stat_tmp) || (j - tracerlayout->timestep < tracerlayout->pad_left + left) || (ii + tracerzoom) >= TRACERBUFFERSIZE)
		{
			if (j - tracerlayout->timestep < tracerlayout->pad_left + left)
				j = tracerlayout->pad_left + left;
			SelectObject(hDC, hBrushMirakel[BRUSH_LIGHTGRAY]);
			SelectObject(hDC, hPenMirakel[BRUSH_LIGHTGRAY]);
			switch (tracer[trace]->type)
			{
			case Fase:
				if (rs_stat_tmp == 1)
				{
					SelectObject(hDC, hBrushMirakel[BRUSH_RED]);
					SelectObject(hDC, hPenMirakel[BRUSH_LIGHTGRAY]);
				}
				if (rs_stat_tmp == 2)
				{
					SelectObject(hDC, hBrushMirakel[BRUSH_DARKGREEN]);
					SelectObject(hDC, hPenMirakel[BRUSH_DARKGREEN]);
				}
				if (rs_stat_tmp == 3)
				{
					SelectObject(hDC, hBrushMirakel[BRUSH_GREEN]);
					SelectObject(hDC, hPenMirakel[BRUSH_GREEN]);
				}
				if (rs_stat_tmp == 1)
					Rectangle(hDC, j, log_pad_top_tmp, j + 3, log_pad_top_tmp + tracerlayout->cg_height);
				else
					Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->cg_height, rs_pos_tmp, log_pad_top_tmp + tracerlayout->rs_height + tracerlayout->cg_height);
				break;
			case Counter:
				if (rs_stat_tmp == 1) SelectObject(hDC, hBrushMirakel[BRUSH_RED]), SelectObject(hDC, hPenMirakel[BRUSH_RED]);
				if (rs_stat_tmp == 2) SelectObject(hDC, hBrushMirakel[BRUSH_GREEN]), SelectObject(hDC, hPenMirakel[BRUSH_GREEN]);
				Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->cg_height, j + tracerlayout->timestep, log_pad_top_tmp + tracerlayout->rs_height + tracerlayout->cg_height);
				break;
			case Detector:
			case Hulpelement:
			case Uitgang:
			case Ingang:
			case Memoryelem:
				if (rs_stat_tmp == 1) SelectObject(hDC, hBrushMirakel[BRUSH_LIGHTBLUE]), SelectObject(hDC, hPenMirakel[BRUSH_LIGHTBLUE]);
				if (rs_stat_tmp == 2) SelectObject(hDC, hBrushMirakel[BRUSH_GREEN]), SelectObject(hDC, hPenMirakel[BRUSH_GREEN]);
				if (rs_stat_tmp == 3) SelectObject(hDC, hBrushMirakel[BRUSH_RED]), SelectObject(hDC, hPenMirakel[BRUSH_RED]);
				Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->cg_height, rs_pos_tmp, log_pad_top_tmp + tracerlayout->rs_height + tracerlayout->cg_height);
				break;
			case Timer:
				if (rs_stat_tmp == 1) SelectObject(hDC, hBrushMirakel[BRUSH_LIGHTBLUE]), SelectObject(hDC, hPenMirakel[BRUSH_LIGHTBLUE]);
				if (rs_stat_tmp == 2) SelectObject(hDC, hBrushMirakel[BRUSH_BLUE]), SelectObject(hDC, hPenMirakel[BRUSH_BLUE]);
				if (rs_stat_tmp == 3) SelectObject(hDC, hBrushMirakel[BRUSH_DARKRED]), SelectObject(hDC, hPenMirakel[BRUSH_DARKRED]);
				if (rs_stat_tmp == 4) SelectObject(hDC, hBrushMirakel[BRUSH_RED]), SelectObject(hDC, hPenMirakel[BRUSH_RED]);
				Rectangle(hDC, j, log_pad_top_tmp + tracerlayout->cg_height, rs_pos_tmp, log_pad_top_tmp + tracerlayout->rs_height + tracerlayout->cg_height);
				break;
			default:
				break;
			}
		}
	}
	/* Weergeven labels */
	//TextOut(hDC, left + mir_i_char_w + 20, log_pad_top_tmp + tracerlayout->fc_height / 15, FC_code[_fc], min(6, strlen(FC_code[_fc])));
	int labelwidth = 50;
	SetBkMode(hDC, TRANSPARENT);
	switch (tracer[trace]->type)
	{
	case Fase:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, FC_code[tracer[trace]->elem], min(10, strlen(FC_code[tracer[trace]->elem])));
		break;
	case Detector:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, D_code[tracer[trace]->elem], min(10, strlen(D_code[tracer[trace]->elem])));
		break;
	case Counter:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, C_code[tracer[trace]->elem], min(10, strlen(C_code[tracer[trace]->elem])));
		break;
	case Timer:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, T_code[tracer[trace]->elem], min(10, strlen(T_code[tracer[trace]->elem])));
		break;
	case Hulpelement:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, H_code[tracer[trace]->elem], min(10, strlen(H_code[tracer[trace]->elem])));
		break;
	case Ingang:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, IS_code[tracer[trace]->elem], min(10, strlen(IS_code[tracer[trace]->elem])));
		break;
	case Uitgang:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, US_code[tracer[trace]->elem], min(10, strlen(US_code[tracer[trace]->elem])));
		break;
	case Memoryelem:
		TextOut(hDC, right - labelwidth, log_pad_top_tmp + tracerlayout->fc_height / 5 - 1, MM_code[tracer[trace]->elem], min(10, strlen(MM_code[tracer[trace]->elem])));
		break;
	default:
		break;
	}
	SetBkMode(hDC, OPAQUE);
}
