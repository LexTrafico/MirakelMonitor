#include "TabSwitches.h"

static HFONT hFont;
static HWND hWndSwitchesDataGrid;
static short * SCH_set;
static short * SCH_cur;
static short * SCH_orig;

DWORD WINAPI AddRemainingSwitches(int rtm)
{
	char temp[3][32];
	int i;

	for (int sch = rtm; sch < SCH_MAX; ++sch)
	{
		strcpy_s(temp[0], 32, SCH_code[sch]);
		sprintf_s(temp[1], 32, "%d", SCH[sch]);
		DataGridView_AddRow(hWndSwitchesDataGrid, (LPSTR)temp, 2);
	}

	return 0;
}

LRESULT CALLBACK WindowProcTabSwitches(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect, rect2;
	char temp[3][32];
	int i, col, row;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hWndSwitchesDataGrid = CreateWindowEx((DWORD)NULL, "DGridVwClass",
			"Switches",
			WS_CHILD, rect.left, rect.top, rect.right - rect.left - 6, rect.bottom - 6, hWnd, NULL, hMainInstance, tabSCH_ischanged);
		DataGridView_AddColumn(hWndSwitchesDataGrid, 1, 150, (LPSTR)"Switches");
		DataGridView_AddColumn(hWndSwitchesDataGrid, 2, 75, (LPSTR)"Instelling");

		SCH_orig = malloc(sizeof(short) * SCH_MAX);
		SCH_cur = malloc(sizeof(short) * SCH_MAX);
		SCH_set = malloc(sizeof(short) * SCH_MAX);

		int sch = 0;
		for (sch = 0; sch < SCH_MAX; ++sch)
		{
			SCH_set[sch] = 0;
			SCH_cur[sch] = SCH_orig[sch] = SCH[sch];
		}
		for (sch = 0; sch < SCH_MAX; ++sch)
		{
			strcpy_s(temp[0], 32, SCH_code[sch]);
			sprintf_s(temp[1], 32, "%d", SCH[sch]);
			DataGridView_AddRow(hWndSwitchesDataGrid, (LPSTR)temp, 2);
			ListView_GetItemRect(hWndSwitchesDataGrid, sch, &rect2, LVIR_BOUNDS);
			SCH_orig[sch] = SCH[sch];
			if (rect2.bottom >= rect.bottom - 6) break;
		}
		
		CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			AddRemainingSwitches,   // thread function name
			sch + 1,                // argument to thread function 
			0,                      // use default creation flags 
			NULL);                  // returns the thread identifier 

		DataGridView_DisplayRowHeaders(hWndSwitchesDataGrid, TRUE);
		DataGridView_ExtendLastColumn(hWndSwitchesDataGrid, TRUE, 400, 100);
		DataGridView_SetResizableHeader(hWndSwitchesDataGrid, 1);
		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);

		if (hWndSwitchesDataGrid)
		{
			SendMessage(hWndSwitchesDataGrid, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right - rect.left - 6, rect.bottom - rect.top - 6));
			InvalidateRect(hMainTab, &rect, TRUE);
		}
		break;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hWndSwitchesDataGrid, SW_SHOW);
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
			DataGridView_GetCellValue(hWndSwitchesDataGrid, row, col, temp[0], 32);
			if (strlen(temp[0]) > 1) 
			{
				SCH_set[row] = 2;
				return 1;
			}
			for (i = 0; i < 1; i++)
			{
				if (temp[0][i] >= 0 && temp[0][i] <= 255 && !isdigit(temp[0][i]) || atoi(temp[0]) > 1)
				{
					SCH_set[row] = 2;
					return 1;
				}
			}
			SCH_cur[row] = SCH[row] = atoi(temp[0]);
			SCH_set[row] = (atoi(temp[0]) != SCH_orig[row]) ? 1 : 0;
			return 1;
		}
		break;

	case WM_DESTROY:
		free(SCH_set);
		free(SCH_cur);
		free(SCH_orig);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabSwitchesUpdate()
{
	RECT rect;
	if (CIF_PARM1WIJZPB != -1 || CIF_PARM1WIJZAP != -1 ||
		CIF_PARM2WIJZPB != -1 || CIF_PARM2WIJZAP != -1)
	{
		for (int sch = 0; sch < SCH_MAX; ++sch)
		{
			if (SCH_cur[sch] != SCH[sch])
			{
				char temp[32];
				SCH_cur[sch] = SCH[sch];
				SCH_set[sch] = (SCH[sch] != SCH_orig[sch]) ? 1 : 0;
				sprintf_s(temp, 32, "%d", SCH[sch]);
				DataGridView_SetCellValue(hWndSwitchesDataGrid, sch, 1, temp);
				ListView_GetItemRect(hWndSwitchesDataGrid, sch, &rect, LVIR_BOUNDS);
				InvalidateRect(hWndSwitchesDataGrid, &rect, TRUE);
			}
		}
	}
}

void TabSwitchesReset()
{

}

short CALLBACK tabSCH_ischanged(int sch)
{
	return SCH_set[sch];
}