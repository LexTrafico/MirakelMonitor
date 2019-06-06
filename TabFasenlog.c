#include "TabFasenlog.h"

static int iFasenLogTopHeight = 5;
static double dHorZoom = 1.0;
static double dHorZoomSteps = 50.0;
static double dHorZoomStep = 0.03;
static double dHorZoomPage = 0.33;
static double dHorZoomMin = 0.33;
static double dHorZoomMax = 3.0;
static double dVerZoom = 15;
static double dVerZoomStep = 1;
static double dVerZoomPage = 2;
static double dVerZoomMin = 10;
static double dVerZoomMax = 20;
static HDC memHDC;
static HBITMAP bufBMP;
static HWND hWndTrackZoomHorizontal;
static HWND hWndTrackZoomVertical;
static double dLogHorZoom = 2;
static double dLogVerZoom = 15;

static HPEN fasenlog_pens[FL_PEN_MAX];
static HFONT fasenlog_fonts[FL_FONT_MAX];
static int iFasenLogVerScrollPos;
static int iFasenLogVerScrollMax;
static int iFasenLogVerScrollMin;

HWND hFasenTree;// listbox met fasen

LRESULT CALLBACK WindowProcTabFasenlog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HDC hDC;
	PAINTSTRUCT ps;
	HDC memHdc = NULL;
	const int fcheight = 30;
	int logheight = 0;

	const int scrollmin = 0;
	const int scrollmax = 3600;
	static int scrollpos = 3600;

	switch (uMsg)
	{
	// Window creation
	case WM_CREATE:

		GetClientRect(hWnd, &rect);
		hFasenTree = CreateWindow(WC_TREEVIEW,
			"MirakelFasenTreeSelect",
			WS_CHILD | TVS_CHECKBOXES | TVS_HASBUTTONS | TVS_NOHSCROLL | WS_VISIBLE | WS_BORDER,
			rect.left + 3, rect.top + 3, 110, rect.bottom - 6,
			hWnd, NULL, hMainInstance, NULL);

		Controller->fasenlog = (FASENLOGSTRUCT *)malloc(sizeof(FASENLOGSTRUCT) * FC_MAX);
		Controller->hFC = (HTREEITEM *)malloc(sizeof(HTREEITEM) * FC_MAX);

		for (int i = 0; i < FC_MAX; ++i)
		{
			Controller->fasenlog[i].id = 1;
			Controller->fasenlog[i].fc_code[0] = '\0';
			for (int j = 0; j < FCLOGMAX; ++j)
			{
				Controller->fasenlog[i].buffer[j].aanvr = 0;
				Controller->fasenlog[i].buffer[j].ar = 0;
				Controller->fasenlog[i].buffer[j].timestamp = 0;
				Controller->fasenlog[i].buffer[j].state = _CG_NO;
			}
		}

		Controller->hFasen = AddItemToTree(hFasenTree, "Fasen", NULL, -1);
		Controller->hAuto = AddItemToTree(hFasenTree, "Auto", Controller->hFasen, -2);
		Controller->hFiets = AddItemToTree(hFasenTree, "Fiets", Controller->hFasen, -3);
		Controller->hVtg = AddItemToTree(hFasenTree, "Voetg.", Controller->hFasen, -4);
		Controller->hOV = AddItemToTree(hFasenTree, "OV", Controller->hFasen, -5);

		for (int i = 0; i < FC_MAX; ++i)
		{
			LPCSTR tempfc = (LPTSTR)MirGiveCCOLString(TYPE_FC, i);
			// kopieren fc naam in fasenlogstruct
			strcpy_s(Controller->fasenlog[i].fc_code, 4, tempfc);
			int fc = atoi(tempfc);
			if (fc > 0 && fc <= 20 || fc > 60 && fc <= 80)
				Controller->hFC[i] = AddItemToTree(hFasenTree, (LPTSTR)tempfc, Controller->hAuto, i);
			else if (fc > 20 && fc <= 30 || fc > 80 && fc <= 90)
				Controller->hFC[i] = AddItemToTree(hFasenTree, (LPTSTR)tempfc, Controller->hFiets, i);
			else if (fc > 30 && fc <= 40)
				Controller->hFC[i] = AddItemToTree(hFasenTree, (LPTSTR)tempfc, Controller->hVtg, i);
			else if (fc > 40 && fc <= 60)
				Controller->hFC[i] = AddItemToTree(hFasenTree, (LPTSTR)tempfc, Controller->hOV, i);
		}

		TreeView_Expand(hFasenTree, Controller->hFasen, TVE_EXPAND);
		TreeView_Expand(hFasenTree, Controller->hAuto, TVE_EXPAND);
		TreeView_Expand(hFasenTree, Controller->hFiets, TVE_EXPAND);
		TreeView_Expand(hFasenTree, Controller->hVtg, TVE_EXPAND);
		TreeView_Expand(hFasenTree, Controller->hOV, TVE_EXPAND);

		// Device context maken voor double buffer painting
		hDC = GetDC(hWnd);
		memHDC = CreateCompatibleDC(hDC);
		GetClientRect(hWnd, &rect);
		bufBMP = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
		SelectObject(memHDC, bufBMP);

		// create all pens
		fasenlog_pens[FL_PEN_TIMELINES] = CreatePen(PS_SOLID, 1, RGB(190, 190, 190));

		// create all fonts
		fasenlog_fonts[FL_FONT_TIMESTAMP] = CreateFont(13, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Arial");
		fasenlog_fonts[FL_FONT_LABEL] = CreateFont(17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");

		// set scroll pos in control to maximum
		SetScrollRange(hWnd, SB_HORZ, scrollmin, scrollmax, TRUE);
		SetScrollPos(hWnd, SB_HORZ, scrollpos, TRUE);

		return 0;

	// Sizing
	case WM_SIZE:
		DeleteDC(memHDC);
		DeleteObject(bufBMP);
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		hDC = GetDC(hWnd);
		memHDC = CreateCompatibleDC(hDC);
		bufBMP = CreateCompatibleBitmap(hDC, LOWORD(lParam), HIWORD(lParam));
		SelectObject(memHDC, bufBMP);

		SetWindowPos(hFasenTree, NULL,
			rect.left + 3, rect.top + 3, 110, rect.bottom - 6,
			SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
		/*SetWindowPos(hFasenLog, NULL,
			rect.left + 117, rect.top + 34, rect.right - 121, rect.bottom - 7 - 30,
			SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);*/
		break;

	// Vertical scrolling
	case WM_VSCROLL:

		GetScrollRange(hWnd, SB_VERT, &iFasenLogVerScrollMin, &iFasenLogVerScrollMax);
		// Get all the vertial scroll bar information.
		//si.cbSize = sizeof(si);
		//si.fMask = SIF_ALL;
		//GetScrollInfo(hWnd, SB_VERT, &si);

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		//SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		//GetScrollInfo(hWnd, SB_VERT, &si);

		//// If the position has changed, scroll window and update it.
		//if (si.nPos != yPos)
		//{
		//	ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
		//	UpdateWindow(hwnd);
		//}

		//SetScrollPos(hWnd, SB_VERT, iFasenLogVerScrollMax, TRUE);
		//SetScrollRange(hWnd, SB_VERT, 0, iFasenLogVerScrollMax, TRUE);
		//SetScrol
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iFasenLogVerScrollPos = iFasenLogVerScrollPos - 1 < 0 ? 0 : iFasenLogVerScrollPos - 1;
			break;
		case SB_LINEDOWN:
			iFasenLogVerScrollPos = iFasenLogVerScrollPos + 1 > iFasenLogVerScrollMax ? iFasenLogVerScrollMax : iFasenLogVerScrollPos + 1;
			break;
		case SB_PAGEUP:
			iFasenLogVerScrollPos = iFasenLogVerScrollPos - 10 < 0 ? iFasenLogVerScrollPos : iFasenLogVerScrollPos - 10;
			break;
		case SB_PAGEDOWN:
			iFasenLogVerScrollPos = iFasenLogVerScrollPos + 10 > iFasenLogVerScrollMax ? iFasenLogVerScrollMax : iFasenLogVerScrollPos + 10;
			break;
		case SB_THUMBTRACK:
			iFasenLogVerScrollPos = HIWORD(wParam);
			break;
		default:
			break;
		}
		SetScrollPos(hWnd, SB_VERT, iFasenLogVerScrollPos, TRUE);
		return 0;

		// Horizontal scrolling
	case WM_HSCROLL:
		SetScrollRange(hWnd, SB_HORZ, scrollmin, scrollmax, TRUE);
		switch (LOWORD(wParam))
		{
			//GetScrollRange(hWnd, SB_HORZ, &scrollmin, &scrollmax);
		case SB_LINELEFT:
			scrollpos = scrollpos - 1 < scrollmin ? scrollmin : scrollpos - 1;
			break;
		case SB_LINERIGHT:
			scrollpos = scrollpos + 1 > scrollmax ? scrollmax : scrollpos + 1;
			break;
		case SB_PAGELEFT:
			scrollpos = scrollpos - 30 < scrollmin ? scrollmin : scrollpos - 30;
			break;
		case SB_PAGERIGHT:
			scrollpos = scrollpos + 30 > scrollmax ? scrollmax : scrollpos + 30;
			break;
		case SB_THUMBTRACK:
			scrollpos = HIWORD(wParam);
			break;
		default:
			break;
		}
		scrollpos = max(0, scrollpos);
		SetScrollPos(hWnd, SB_HORZ, scrollpos, TRUE);

		return 0;
	case WM_PAINT:
	{
		// Erase background
		GetClientRect(hWnd, &rect);
		SelectObject(memHDC, hBrushCG[_CG_BG]);
		SelectPen(memHDC, GetStockObject(NULL_PEN));
		Rectangle(memHDC, rect.left, rect.top, rect.right, rect.bottom);

		// Set background mode
		SetBkMode(memHDC, TRANSPARENT);

		logheight += fcheight;
		for (int fc = iFasenLogVerScrollPos, fcp = 0; fc < FC_MAX; ++fc)
		{
			//if (TreeView_GetCheckState(hFasenTree, Controller->hFC[fc]) > 0)
			//{
				fasenlog_paintfc(memHDC,						// Device context
					Controller,							// Pointer to controller struct 
					fc,							// Phase index
					dLogHorZoom,					// Pixels per TE
					5,							// X
					iFasenLogTopHeight + 20 + (int)(dLogVerZoom + 4) * fcp++,		// Y
					rect.right - 15,			// Width
					(int)(dLogVerZoom),			// Height of bars
					scrollmax - scrollpos);		// Horizontal scroll pos
				logheight += fcheight;
			//}
		}
		fasenlog_painttimebars(memHDC,						// Device context
			Controller, 						// Pointer to controller struct
			dLogHorZoom,					// Pixels per TE
			20, 						// X
			iFasenLogTopHeight,					// Y
			rect.right - 27, 			// Width
			logheight, 					// Height
			scrollmax - scrollpos);		// Horizontal scroll pos

/* Buffer kopieren naar scherm dc */
		hDC = BeginPaint(hWnd, &ps);
		BitBlt(hDC, 0, 0, rect.right, rect.bottom, memHDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_NOTIFY:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
static BOOL fasenlog_entry_differ(const struct fasenlogentry * fle_1, const struct fasenlogentry * fle_2)
{
	if (fle_1->aanvr != fle_2->aanvr)		return TRUE;
	if (fle_1->ar != fle_2->ar)			return TRUE;
	if (fle_1->state != fle_2->state)		return TRUE;
	return FALSE;
}

static void fasenlog_entry_copy(struct fasenlogentry * fle_dest, const struct fasenlogentry * fle_src)
{
	fle_dest->aanvr = fle_src->aanvr;
	fle_dest->ar = fle_src->ar;
	fle_dest->timestamp = fle_src->timestamp;
	fle_dest->state = fle_src->state;
}

void fasenlog_update(CONTROLLERSTRUCT * ct)
{
	int i;
	struct fasenlogentry * fle;

	// update time
	Controller->cttime = MirGetCTTime();
	Controller->cttime_val = (Controller->cttime & 0xff) +
		(10 * ((Controller->cttime >> 8) & 0xff)) +
		(600 * ((Controller->cttime >> 16) & 0xff)) +
		(36000 * ((Controller->cttime >> 24) & 0xff));

	// update fase status
	for (i = 0; i < FC_MAX; ++i)
	{
		fle = MirGetCCOLFCState(i);
		if (fasenlog_entry_differ(fle, &Controller->fasenlog[i].buffer[Controller->fasenlog[i].id]))
		{
			fle->timestamp = Controller->cttime;
			Controller->fasenlog[i].id = ((Controller->fasenlog[i].id + 1) % FCLOGMAX);
			fasenlog_entry_copy(&Controller->fasenlog[i].buffer[Controller->fasenlog[i].id], fle);
		}
	}
}

// TODO:
// opslaan welke fasen geselecteerd zijn
// fc details weer kunnen geven
// overgang om 00:00:00 fixen
// weergave blokken overgangen
void fasenlog_paintfc(HDC hDC,						// Device context
	struct mirakel_controller * ct,	// Pointer to controller struct
	int fc,							// Phase number
	double ppte,					// Pixel per TE
	int x,							// Position X
	int y,							//          Y
	int w,							//          Width
	int h,							//          Height
	int hor_scroll_pos)				// Horizontal scroll position
{
	unsigned long fclogtime, pfclogtime = 0;
	char temp[256];

	unsigned long scrolled_time, scrolled_time_offset = 0;

	// check aanwezigheid controller
	if (ct)
	{
		RECT rect;
		double X_current = 0,				// Current X
			X_previous = 0,				// Previous X
			X_previous_corrected = 0,	// Corrected previous X
			fc_width = w - 17;			// Phase width

		int uren, puren;

		// tid = time ID, checkid = looper var
		// met tid beginnen 1tje terug: huidige is voor volgende logentry
		int tid = Controller->fasenlog[fc].id, checkid = 0;

		// links en rechts fasen naam schilderen
		SelectFont(hDC, fasenlog_fonts[FL_FONT_LABEL]);

		rect.left = w + x - 15;
		rect.right = w + x;
		rect.top = y - 1;
		rect.bottom = y + h;
		DrawText(hDC, Controller->fasenlog[fc].fc_code, strlen(Controller->fasenlog[fc].fc_code), &rect, DT_LEFT | DT_CENTER | DT_VCENTER | DT_NOCLIP);
		rect.left = x;
		rect.right = x + 15;
		DrawText(hDC, Controller->fasenlog[fc].fc_code, strlen(Controller->fasenlog[fc].fc_code), &rect, DT_LEFT | DT_CENTER | DT_VCENTER | DT_NOCLIP);
		//TextOut(hDC, w + x - 15, y - (int)((double)h / 12) + 1, Controller->fasenlog[fc].fc_code, strlen(Controller->fasenlog[fc].fc_code));
		//TextOut(hDC, x, y + (int)((double)h / 8 + 2), Controller->fasenlog[fc].fc_code, strlen(Controller->fasenlog[fc].fc_code));

		// indien huidig ID lager dan 0, zet op maximaal
		if (tid < 0) tid = FCLOGMAX - 1;
		// tijd waarde eerste uit te lezen entry bepalen
		fclogtime = (Controller->fasenlog[fc].buffer[tid].timestamp & 0xff) +
			(10 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 8) & 0xff)) +
			(600 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 16) & 0xff)) +
			(36000 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 24) & 0xff));
		uren = ((Controller->fasenlog[fc].buffer[tid].timestamp >> 24) & 0xff);

		// Determine scrolled time & offset
		scrolled_time_offset = hor_scroll_pos * 10;
		scrolled_time = Controller->cttime_val - hor_scroll_pos * 10;
		if (scrolled_time < 0) scrolled_time += 36000;

		// beginnen bij huidige tijdstamp
		puren = ((Controller->cttime >> 24) & 0xff);
		pfclogtime = Controller->cttime_val;

		// TODO: below would be more efficient, but causes a problem with scrolling
		// in addition to below, offset should be set to 0 for this to 'work'.
		// todo = find out why things start to  shift when using this

		// Find the first entry that falls inside the range to be drawn
		//while ((checkid++ < FCLOGMAX) && (fclogtime > scrolled_time))
		//{
		//	--tid;
		//	if (tid < 0) tid = FCLOGMAX - 1;
		//	pfclogtime = fclogtime;
		//	fclogtime = (Controller->fasenlog[fc].buffer[tid].timestamp & 0xff) +
		//		(10 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 8) & 0xff)) +
		//		(600 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 16) & 0xff)) +
		//		(36000 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 24) & 0xff));
		//}
		//
		//checkid--;
		//++tid;
		//if (tid >= FCLOGMAX) tid -= FCLOGMAX;
		//fclogtime = pfclogtime;
		//pfclogtime = scrolled_time;

		// - beginnen rechts (w), steeds verder naar links (cx = current x)
		// - positie is x en y
		// - tijd steeds een logpositie terug
		while ((checkid++ < FCLOGMAX) &&		// niet meer dan volledige buffer uitlezen
			((X_current + 20.0) < fc_width))	// de huidige cx is kleiner dan de breedte
		{

			// apply scrolled offset
			fclogtime += scrolled_time_offset;

			// reset pen
			SelectPen(hDC, GetStockObject(NULL_PEN));

			// huidige x bepalen
			if (uren == 23 && puren == 0 && fclogtime > pfclogtime)
				X_current += (((double)(pfclogtime + 864000L) - (double)(fclogtime)) * ppte);
			// WTFWTFWTF...
			//else if (fclogtime > pfclogtime)
			//{
			//	X_current += (((double)fclogtime+(double)(pfclogtime)-(double)(scrolled_time)) * ppte);
			//}
			else
				X_current += (((double)pfclogtime - (double)fclogtime) * ppte);

			if ((fc_width - X_current - 20) < 0)
				X_current = fc_width - 20;

			// draw only when inside the drawing range!
			if ((X_current + 20) <= fc_width)
			{

				if ((X_previous + 20) >= fc_width) X_previous_corrected = fc_width - 20;
				else X_previous_corrected = X_previous;

				if (Controller->fasenlog[fc].buffer[tid].state == _CG_NO)
					SelectObject(hDC, hBrushCG[_CG_NO]);
				else
					SelectObject(hDC, hBrushCG[Controller->fasenlog[fc].buffer[tid].state]);

				// int casts
				int iX_previous_corrected = (int)X_previous_corrected;
				int iX_current = (int)X_current;
				int ifc_width = (int)fc_width;

				// drawing!
				if (Controller->fasenlog[fc].buffer[tid].state >= _CG_RV)
				{
					// tijdelijke y: midden van balkje
					int ty = y + (int)(((double)h) / 2.0);
					// lijntje
					Rectangle(hDC,
						ifc_width - iX_previous_corrected + x,
						ty - 1,
						ifc_width - iX_current + x - 1,
						ty + 1);
					// aanvraag verklikken
					if (Controller->fasenlog[fc].buffer[tid].aanvr && !(fclogtime == pfclogtime))
					{
						SelectObject(hDC, hBrushCG[_CG_VS]);
						Rectangle(hDC,
							// rechter x: breedte - "huidige" x + basis x
							ifc_width - iX_current + x + 2,
							y,
							// linker x: breedte - "huidge" x + basis x - verschil met vorige tijd keer pixels per te
							ifc_width - iX_current + x - 1,
							y + h);
					}
				}
				else
				{
					Rectangle(hDC,
						// linker x: breedte - "huidge" x + basis x - verschil met vorige tijd keer pixels per te
						ifc_width - iX_current + x - 1,
						y,
						// rechter x: breedte - "huidige" x + basis x
						ifc_width - iX_previous_corrected + x,
						y + h);
					if (Controller->fasenlog[fc].buffer[tid].ar)
						SelectObject(hDC, hBrushCG[_CG_VG]);
					else
						SelectObject(hDC, hBrushCG[_CG_FG]);
					Rectangle(hDC,
						// linker x: breedte - "huidge" x + basis x - verschil met vorige tijd keer pixels per te
						ifc_width - iX_current + x - 1,
						y + h - 1,
						// rechter x: breedte - "huidige" x + basis x
						ifc_width - iX_previous_corrected + x,
						y + h + 2);
				}

				// prev X opslaan
				X_previous = X_current;

				if ((fc_width - X_previous) < 0) X_previous = fc_width;

				--tid;
				if (tid < 0) tid = FCLOGMAX - 1;
				pfclogtime = fclogtime;
				puren = uren;
				fclogtime = (Controller->fasenlog[fc].buffer[tid].timestamp & 0xff) +
					(10 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 8) & 0xff)) +
					(600 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 16) & 0xff)) +
					(36000 * ((Controller->fasenlog[fc].buffer[tid].timestamp >> 24) & 0xff));
				uren = ((Controller->fasenlog[fc].buffer[tid].timestamp >> 24) & 0xff);
			}
		}
		tid = Controller->fasenlog[fc].id;
	}
}

// TODO
// - dit moet mee scrollen
void fasenlog_painttimebars(HDC hDC, struct mirakel_controller * ct, float ppte, int x, int y, int w, int h, int scrollp)
{
	int checker = w;
	char temp[64];
	float bars = (float)w;
	int uren = ((Controller->cttime >> 24) & 0xff) - (scrollp / 3600);
	int minuten = ((Controller->cttime >> 16) & 0xff) - ((scrollp % 3600) / 60);
	int seconden = ((Controller->cttime >> 8) & 0xff) - (scrollp % 60);
	int tienden = ((Controller->cttime) & 0xff) - (scrollp % 10);
	if (seconden < 0) { seconden += 60; minuten -= 1; }
	if (minuten < 0) { minuten += 60; uren -= 1; }
	if (uren < 0) uren += 24;

	// timestamp font laden
	SelectFont(hDC, fasenlog_fonts[FL_FONT_TIMESTAMP]);

	// offset: zorgen dat lijnen en labels op hele 10 sec komen
	bars -= (float)(((seconden % 10) * 10) + tienden) * ppte;
	seconden -= (seconden % 10);

	// eerste label: mag al verschijnen voor de eerste lijn
	if (0) {
		int _seconden = seconden + 10;
		int _minuten = minuten;
		int _uren = uren;
		float _bars = bars + (100 * ppte);
		if (_seconden >= 60) { _seconden -= 60; _minuten += 1; }
		if (_minuten >= 60) { _minuten -= 60; _uren += 1; }
		if (_uren >= 24) _uren -= 24;
		sprintf_s(temp, 64, "%2.2d:%2.2d:%2.2d", _uren, _minuten, _seconden);
		TextOut(hDC, (int)bars, y, temp, strlen(temp));
	}

	bars += (100 * ppte);
	seconden += 10;
	if (seconden >= 60) { seconden -= 60; minuten += 1; }
	if (minuten >= 60) { minuten -= 60; uren += 1; }
	if (uren >= 24) uren -= 24;
	sprintf_s(temp, 64, "%2.2d:%2.2d:%2.2d", uren, minuten, seconden);
	TextOut(hDC, (int)bars, y, temp, strlen(temp));


	// terugzetten waarden zodat de eerste zichtbare lijn + label getekend wordt
	bars -= (100 * ppte);
	seconden -= 10;
	if (seconden < 0) { seconden += 60; minuten -= 1; }
	if (minuten < 0) { minuten += 60; uren -= 1; }
	if (uren < 0) uren += 24;

	SelectPen(hDC, fasenlog_pens[FL_PEN_TIMELINES]);

	// doorlopen venster vrnl
	while (bars > 25)
	{
		// elke 10 seconden
		if (((checker--) % 10) == 0)
		{
			MoveToEx(hDC, (int)bars, y, NULL);
			LineTo(hDC, (int)bars, y + h);
			sprintf_s(temp, 64, "%2.2d:%2.2d:%2.2d", uren, minuten, seconden);
			TextOut(hDC, (int)bars, y, temp, strlen(temp));
			bars -= (100 * ppte);
			seconden -= 10;
			if (seconden < 0) { seconden += 60; minuten -= 1; }
			if (minuten < 0) { minuten += 60; uren -= 1; }
			if (uren < 0) uren += 24;
		}
	}
	// laatste label mag voorbij laatste lijn
	sprintf_s(temp, 64, "%2.2d:%2.2d:%2.2d", uren, minuten, seconden);
	TextOut(hDC, (int)bars, y, temp, strlen(temp));
}