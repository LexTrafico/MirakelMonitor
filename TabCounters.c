#include "TabCounters.h"

static HFONT hFont;
static HWND hWndCountersDataGrid;
static short * C_counter_old;
static RECT * C_counter_rect;
static short * C_orig;
static short * C_cur;
static short * C_set;

DWORD WINAPI AddRemainingCounters(int rtm)
{
	char temp[3][32];
	int i;

	Sleep(500);
	for (int ct = rtm; ct < CT_MAX; ++ct)
	{
		strcpy_s(temp[0], 32, C_code[ct]);
		sprintf_s(temp[1], 32, "%d", C_max[ct]);
		sprintf_s(temp[2], 32, "%d", C_counter[ct]);
		DataGridView_AddRow(hWndCountersDataGrid, (LPSTR)temp, 3);
	}

	return 0;
}

LRESULT CALLBACK WindowProcTabCounters(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	char temp[3][32];
	int i, col, row;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hWndCountersDataGrid = CreateWindowEx((DWORD)NULL, "DGridVwClass",
			"Counters",
			WS_CHILD, rect.left, rect.top, rect.right - rect.left - 6, rect.bottom - 6, hWnd, NULL, hMainInstance, tabCT_ischanged);
		DataGridView_AddColumn(hWndCountersDataGrid, 1, 150, (LPSTR)"Counters");
		DataGridView_AddColumn(hWndCountersDataGrid, 2, 75, (LPSTR)"Instelling");
		DataGridView_AddColumn(hWndCountersDataGrid, 3, 75, (LPSTR)"Actueel");

		C_counter_old = malloc(sizeof(short) * CT_MAX);
		C_counter_rect = malloc(sizeof(RECT) * CT_MAX);
		C_orig = malloc(sizeof(short) * CT_MAX);
		C_cur = malloc(sizeof(short) * CT_MAX);
		C_set = malloc(sizeof(short) * CT_MAX);

		int ct = 0;
		for (ct = 0; ct < CT_MAX; ++ct)
		{
			C_orig[ct] = C_max[ct];
			C_cur[ct] = C_max[ct];
			C_set[ct] = 0;
		}
		for (ct = 0; ct < CT_MAX; ++ct)
		{
			strcpy_s(temp[0], 32, C_code[ct]);
			sprintf_s(temp[1], 32, "%d", C_max[ct]);
			sprintf_s(temp[2], 32, "%d", C_counter[ct]);
			DataGridView_AddRow(hWndCountersDataGrid, (LPSTR)temp, 3);
			ListView_GetItemRect(hWndCountersDataGrid, ct, &C_counter_rect[ct], LVIR_BOUNDS);
			if (C_counter_rect[ct].bottom >= rect.bottom - 6) break;
		}
		CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			AddRemainingCounters,   // thread function name
			ct + 1,                 // argument to thread function 
			0,                      // use default creation flags 
			NULL);                  // returns the thread identifier 

		DataGridView_DisplayRowHeaders(hWndCountersDataGrid, TRUE);
		DataGridView_ExtendLastColumn(hWndCountersDataGrid, TRUE, 400, 100);
		DataGridView_SetResizableHeader(hWndCountersDataGrid, 1);
		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);

		if (hWndCountersDataGrid)
		{
			SendMessage(hWndCountersDataGrid, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right - rect.left - 6, rect.bottom - rect.top - 6));
			InvalidateRect(hMainTab, &rect, TRUE);
		}
		return 0;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hWndCountersDataGrid, SW_SHOW);
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
			DataGridView_GetCellValue(hWndCountersDataGrid, row, col, temp[0], 32);
			for (i = 0; i < strlen(temp[0]); i++)
			{
				if (temp[0][i] >= 0 && temp[0][i] <= 255 && !isdigit(temp[0][i]))
				{
					C_set[row] = 2;
					return 1;
				}
			}
			C_cur[row] = C_max[row] = atoi(temp[0]);
			C_set[row] = (atoi(temp[0]) != C_orig[row]) ? 1 : 0;
			return 1;
		}
		return 0;

	case WM_DESTROY:
		free(C_orig);
		free(C_set);
		free(C_cur);
		free(C_counter_old);
		free(C_counter_rect);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabCountersUpdate()
{
	char ctemp[8];
	if (!hWndCountersDataGrid) return;

	int c1 = ListView_GetColumnWidth(hWndCountersDataGrid, 0);
	int c2 = ListView_GetColumnWidth(hWndCountersDataGrid, 1);

	int top = ListView_GetTopIndex(hWndCountersDataGrid);
	ListView_GetItemRect(hWndCountersDataGrid, top, &C_counter_rect[top], LVIR_BOUNDS);
	RECT rect;
	GetWindowRect(hWndCountersDataGrid, &rect);

	for (int ct = top; ct < CT_MAX; ++ct)
	{
		ListView_GetItemRect(hWndCountersDataGrid, ct, &C_counter_rect[ct], LVIR_BOUNDS);
		if (C_counter[ct] != C_counter_old[ct] && C_counter_rect[ct].top >= C_counter_rect[top].top && C_counter_rect[ct].bottom <= rect.bottom + 20)
		{
			sprintf_s(ctemp, 8, "%d", C_counter[ct]);
			ListView_SetItemText(hWndCountersDataGrid, ct, 2, ctemp);
		}
		else
		{
			C_counter_old[ct] = C_counter[ct];
		}
	}

	for (int ct = 0; ct < CT_MAX; ++ct)
	{
		if (C_max[ct] != C_cur[ct])
		{
			char temp[32];
			C_cur[ct] = C_max[ct];
			C_set[ct] = (C_max[ct] != C_orig[ct]) ? 1 : 0;
			sprintf_s(temp, 32, "%d", C_max[ct]);
			DataGridView_SetCellValue(hWndCountersDataGrid, ct, 1, temp);
			ListView_GetItemRect(hWndCountersDataGrid, ct, &rect, LVIR_BOUNDS);
			InvalidateRect(hWndCountersDataGrid, &rect, TRUE);

		}
		if (C_counter[ct] != C_counter_old[ct])
		{
			C_counter_old[ct] = C_counter[ct];
		}
	}
}

void TabCountersReset()
{

}

short CALLBACK tabCT_ischanged(int ct)
{
	return C_set[ct];
}