#include "TabMemoryElements.h"

static HFONT hFont;
static HWND hWndMemoryElementsDataGrid;
static short * MM_old;

DWORD WINAPI AddRemainingMemoryElements(int rtm)
{
	char temp[2][32];
	int i;

	Sleep(500);
	for (int ct = rtm; ct < ME_MAX; ++ct)
	{
		strcpy_s(temp[0], 32, MM_code[ct]);
		sprintf_s(temp[1], 32, "%d", MM[ct]);
		DataGridView_AddRow(hWndMemoryElementsDataGrid, (LPSTR)temp, 2);
	}

	return 0;
}

LRESULT CALLBACK WindowProcTabMemoryElements(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect, irect;
	char temp[3][32];
	int i, col, row;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hWndMemoryElementsDataGrid = CreateWindowEx((DWORD)NULL, "DGridVwClass",
			"MemoryElements",
			WS_CHILD, rect.left, rect.top, rect.right - rect.left - 6, rect.bottom - 6, hWnd, NULL, hMainInstance, NULL);
		DataGridView_AddColumn(hWndMemoryElementsDataGrid, 1, 150, (LPSTR)"Memory elementen");
		DataGridView_AddColumn(hWndMemoryElementsDataGrid, 2, 75, (LPSTR)"Actueel");

		MM_old = malloc(sizeof(short) * ME_MAX);

		int me = 0;
		for (me = 0; me < ME_MAX; ++me)
		{
			strcpy_s(temp[0], 32, MM_code[me]);
			sprintf_s(temp[1], 32, "%d", MM[me]);
			DataGridView_AddRow(hWndMemoryElementsDataGrid, (LPSTR)temp, 2);
			ListView_GetItemRect(hWndMemoryElementsDataGrid, me, &irect, LVIR_BOUNDS);
			if (irect.bottom >= rect.bottom - 6) break;
		}
		CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			AddRemainingMemoryElements,   // thread function name
			me + 1,                 // argument to thread function 
			0,                      // use default creation flags 
			NULL);                  // returns the thread identifier 

		DataGridView_DisplayRowHeaders(hWndMemoryElementsDataGrid, TRUE);
		DataGridView_ExtendLastColumn(hWndMemoryElementsDataGrid, TRUE, 400, 100);
		DataGridView_SetResizableHeader(hWndMemoryElementsDataGrid, 1);
		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);

		if (hWndMemoryElementsDataGrid)
		{
			GetClientRect(hWnd, &rect);
			SendMessage(hWndMemoryElementsDataGrid, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right - rect.left - 6, rect.bottom - rect.top - 6));
			InvalidateRect(hMainTab, &rect, TRUE);
		}
		return 0;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hWndMemoryElementsDataGrid, SW_SHOW);
			break;
		}
		return 0;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case DGVM_ENDEDIT:
			return 1;
		}
		return 0;

	case WM_DESTROY:
		free(MM_old);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabMemoryElementsUpdate()
{
	char ctemp[8];
	RECT rect, topRect;
	if (!hWndMemoryElementsDataGrid) return;

	int c1 = ListView_GetColumnWidth(hWndMemoryElementsDataGrid, 0);
	int c2 = ListView_GetColumnWidth(hWndMemoryElementsDataGrid, 1);

	int top = ListView_GetTopIndex(hWndMemoryElementsDataGrid);
	ListView_GetItemRect(hWndMemoryElementsDataGrid, top, &topRect, LVIR_BOUNDS);
	RECT winRect;
	GetWindowRect(hWndMemoryElementsDataGrid, &winRect);

	for (int ct = top; ct < ME_MAX; ++ct)
	{
		ListView_GetItemRect(hWndMemoryElementsDataGrid, ct, &rect, LVIR_BOUNDS);
		if (MM[ct] != MM_old[ct] && rect.top >= topRect.top && rect.bottom <= winRect.bottom + 20)
		{
			sprintf_s(ctemp, 8, "%d", MM[ct]);
			ListView_SetItemText(hWndMemoryElementsDataGrid, ct, 1, ctemp);
		}
		else
		{
			MM_old[ct] = MM[ct];
		}
	}

	for (int ct = 0; ct < ME_MAX; ++ct)
	{
		if (MM[ct] != MM_old[ct])
		{
			MM_old[ct] = MM[ct];
		}
	}
}

void TabMemoryElementsReset()
{

}
