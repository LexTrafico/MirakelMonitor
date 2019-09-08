#include "TabParameters.h"

static HFONT hFont;
static HWND hWndParametersDataGrid;
static short * PRM_orig;
static short * PRM_cur;
static short * PRM_set;

DWORD WINAPI AddRemainingParameters(int rtm)
{
	char temp[3][32];
	int i;

	Sleep(250);
	for (int prm = rtm; prm < PRM_MAX; ++prm)
	{
		strcpy_s(temp[0], 32, PRM_code[prm]);
		sprintf_s(temp[1], 32, "%d", PRM[prm]);
		DataGridView_AddRow(hWndParametersDataGrid, (LPSTR)temp, 2);
	}

	return 0;
}

LRESULT CALLBACK WindowProcTabParameters(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect, rect2;
	char temp[3][32];
	int i, row, col;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hWndParametersDataGrid = CreateWindowEx((DWORD)NULL, "DGridVwClass",
			"Parameters",
			WS_CHILD, rect.left, rect.top, rect.right - rect.left - 6, rect.bottom - 6, hWnd, NULL, hMainInstance, tabPRM_ischanged);
		DataGridView_AddColumn(hWndParametersDataGrid, 1, 150, (LPSTR)"Parameters");
		DataGridView_AddColumn(hWndParametersDataGrid, 2, 75, (LPSTR)"Instelling");
		
		PRM_orig = malloc(sizeof(short) * PRM_MAX);
		PRM_cur = malloc(sizeof(short) * PRM_MAX);
		PRM_set = malloc(sizeof(short) * PRM_MAX);

		int prm = 0;
		for (prm = 0; prm < PRM_MAX; ++prm)
		{
			PRM_orig[prm] = PRM[prm];
			PRM_cur[prm] = PRM[prm];
			PRM_set[prm] = 0;
		}
		for (prm = 0; prm < PRM_MAX; ++prm)
		{
			strcpy_s(temp[0], 32, PRM_code[prm]);
			sprintf_s(temp[1], 32, "%d", PRM[prm]);
			DataGridView_AddRow(hWndParametersDataGrid, (LPSTR)temp, 2);
			ListView_GetItemRect(hWndParametersDataGrid, prm, &rect2, LVIR_BOUNDS);
			if (rect2.bottom >= rect.bottom - 6) break;
		}
		HANDLE thread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			AddRemainingParameters, // thread function name
			prm,                    // argument to thread function 
			0,                      // use default creation flags 
			NULL);                  // returns the thread identifier 

		DataGridView_DisplayRowHeaders(hWndParametersDataGrid, TRUE);
		DataGridView_ExtendLastColumn(hWndParametersDataGrid, TRUE, 400, 100);
		DataGridView_SetResizableHeader(hWndParametersDataGrid, 1);
		return 0;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hWndParametersDataGrid, SW_SHOW);
			break;
		}
		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		GetClientRect(hWnd, &rect);

		if (hWndParametersDataGrid)
		{
			SendMessage(hWndParametersDataGrid, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right - rect.left - 6, rect.bottom - rect.top - 6));
			InvalidateRect(hMainTab, &rect, TRUE);
		}
		break;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case DGVM_ENDEDIT:
			col = HIWORD(lParam);
			row = LOWORD(lParam);
			if (col != 1) return 1;
			DataGridView_GetCellValue(hWndParametersDataGrid, row, col, temp[0], 32);
			for (i = 0; i < strlen(temp[0]); i++)
			{
				if (temp[0][i] >= 0 && temp[0][i] <= 255 && !isdigit(temp[0][i]))
				{
					PRM_set[row] = 2;
					return 1;
				}
			}
			PRM_cur[row] = PRM[row] = atoi(temp[0]);
			PRM_set[row] = (atoi(temp[0]) != PRM_orig[row]) ? 1 : 0;
			return 1;
		}
		break;

	case WM_DESTROY:
		free(PRM_orig);
		free(PRM_cur);
		free(PRM_set);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabParametersUpdate()
{
	RECT rect;
	if (CIF_PARM1WIJZPB != -1 || CIF_PARM1WIJZAP != -1 ||
		CIF_PARM2WIJZPB != -1 || CIF_PARM2WIJZAP != -1)
	{
		for (int prm = 0; prm < PRM_MAX; ++prm)
		{
			if (PRM_cur[prm] != PRM[prm])
			{
				char temp[32];
				PRM_cur[prm] = PRM[prm];
				PRM_set[prm] = (PRM[prm] != PRM_orig[prm]) ? 1 : 0;
				sprintf_s(temp, 32, "%d", PRM[prm]);
				DataGridView_SetCellValue(hWndParametersDataGrid, prm, 1, temp);
				ListView_GetItemRect(hWndParametersDataGrid, prm, &rect, LVIR_BOUNDS);
				InvalidateRect(hWndParametersDataGrid, &rect, TRUE);
			}
		}
	}
}

void TabParametersReset()
{

}

short CALLBACK tabPRM_ischanged(int prm)
{
	return PRM_set[prm];
}