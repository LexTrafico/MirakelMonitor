#include "TabTimers.h"

static HFONT hFont;
static HWND hWndTimersDataGrid;
static short * T_timer_old;
static short * T_set;
static short * T_cur;
static short * T_orig;

DWORD WINAPI AddRemainingTimers(int rtm)
{
	char temp[3][32];
	int i;

	Sleep(250);
	for (int tm = rtm; tm < TM_MAX; ++tm)
	{
		strcpy_s(temp[0], 32, T_code[tm]);
		sprintf_s(temp[1], 32, "%d", T_max[tm]);
		sprintf_s(temp[2], 32, "%d", T_timer[tm]);
		DataGridView_AddRow(hWndTimersDataGrid, (LPSTR)temp, 3);
	}

	return 0;
}

LRESULT CALLBACK WindowProcTabTimers(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect, irect;
	char temp[3][32];
	int i, row, col;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hWndTimersDataGrid = CreateWindowEx((DWORD)NULL, "DGridVwClass",
			"Timers",
			WS_CHILD, rect.left, rect.top, rect.right - rect.left - 6, rect.bottom - 6, hWnd, NULL, hMainInstance, tabTM_ischanged);
		DataGridView_AddColumn(hWndTimersDataGrid, 1, 150, (LPSTR)"Timers");
		DataGridView_AddColumn(hWndTimersDataGrid, 2, 75, (LPSTR)"Instelling");
		DataGridView_AddColumn(hWndTimersDataGrid, 3, 75, (LPSTR)"Actueel");

		T_timer_old = malloc(sizeof(short) * TM_MAX);
		T_set = malloc(sizeof(short) * TM_MAX);
		T_cur = malloc(sizeof(short) * TM_MAX);
		T_orig = malloc(sizeof(short) * TM_MAX);
		
		int tm = 0;
		for (tm = 0; tm < TM_MAX; ++tm)
		{
			T_set[tm] = 0;
			T_cur[tm] = T_orig[tm] = T_max[tm];
		}
		for (tm = 0; tm < TM_MAX; ++tm)
		{
			strcpy_s(temp[0], 32, T_code[tm]);
			sprintf_s(temp[1], 32, "%d", T_max[tm]);
			sprintf_s(temp[2], 32, "%d", T_timer[tm]);
			DataGridView_AddRow(hWndTimersDataGrid, (LPSTR)temp, 3);
			ListView_GetItemRect(hWndTimersDataGrid, tm, &irect, LVIR_BOUNDS);
			if (irect.bottom >= rect.bottom - 6) break;
		}
		CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			AddRemainingTimers,     // thread function name
			tm + 1,                 // argument to thread function 
			0,                      // use default creation flags 
			NULL);                  // returns the thread identifier 
		ShowWindow(hWndTimersDataGrid, SW_SHOW);

		DataGridView_DisplayRowHeaders(hWndTimersDataGrid, TRUE);
		DataGridView_ExtendLastColumn(hWndTimersDataGrid, TRUE, 400, 100);
		DataGridView_SetResizableHeader(hWndTimersDataGrid, 1);
		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);

		if (hWndTimersDataGrid)
		{
			SendMessage(hWndTimersDataGrid, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right - rect.left - 6, rect.bottom - rect.top - 6));
			InvalidateRect(hMainTab, &rect, TRUE);
		}
		break;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hWndTimersDataGrid, SW_SHOW);
			break;
		}
		return 0;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case DGVM_ENDEDIT:
			col = HIWORD(lParam);
			row = LOWORD(lParam);
			if (col != 1) return 1;
			DataGridView_GetCellValue(hWndTimersDataGrid, row, col, temp[0], 32);
			for (i = 0; i < strlen(temp[0]); i++)
			{
				if (temp[0][i] >= 0 && temp[0][i] <= 255 && !isdigit(temp[0][i]))
				{
					T_set[row] = 2;
					return 1;
				}
			}
			T_cur[row] = T_max[row] = atoi(temp[0]);
			T_set[row] = (atoi(temp[0]) != T_orig[row]) ? 1 : 0;
			return 1;
		}
		break;

	case WM_DESTROY:
		free(T_set);
		free(T_orig);
		free(T_cur);
		free(T_timer_old);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabTimersUpdate()
{
	char ctemp[8];
	RECT rect, wrect, irect;

	if (!hWndTimersDataGrid) return;

	//int c1 = ListView_GetColumnWidth(hWndTimersDataGrid, 0);
	//int c2 = ListView_GetColumnWidth(hWndTimersDataGrid, 1);

	int top = ListView_GetTopIndex(hWndTimersDataGrid);
	ListView_GetItemRect(hWndTimersDataGrid, top, &rect, LVIR_BOUNDS);
	GetWindowRect(hWndTimersDataGrid, &wrect);

	for (int tm = top; tm < TM_MAX; ++tm)
	{
		if (T_timer[tm] != T_timer_old[tm])
		{
			ListView_GetItemRect(hWndTimersDataGrid, tm, &irect, LVIR_BOUNDS);
			if (irect.bottom - rect.top <= wrect.bottom + 20)
			{
				sprintf_s(ctemp, 8, "%d", T_timer[tm]);
				DataGridView_SetCellValue(hWndTimersDataGrid, tm, 2, ctemp);
				ListView_GetItemRect(hWndTimersDataGrid, tm, &rect, LVIR_BOUNDS);
				InvalidateRect(hWndTimersDataGrid, &rect, TRUE);
			}
		}
	}

	for (int tm = 0; tm < TM_MAX; ++tm)
	{
		if (T_max[tm] != T_cur[tm])
		{
			char temp[32];
			T_cur[tm] = T_max[tm];
			T_set[tm] = (T_max[tm] != T_orig[tm]) ? 1 : 0;
			sprintf_s(temp, 32, "%d", T_max[tm]);
			DataGridView_SetCellValue(hWndTimersDataGrid, tm, 1, temp);
			ListView_GetItemRect(hWndTimersDataGrid, tm, &rect, LVIR_BOUNDS);
			InvalidateRect(hWndTimersDataGrid, &rect, TRUE);
		}
		T_timer_old[tm] = T_timer[tm];
	}
}

void TabTimersReset()
{

}

short CALLBACK tabTM_ischanged(int tm)
{
	return T_set[tm];
}