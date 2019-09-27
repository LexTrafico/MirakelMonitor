#include "TabHelpElements.h"

static HFONT hFont;
static HWND hWndHelpElementsDataGrid;
static short * H_old;

DWORD WINAPI AddRemainingHelpElements(int rtm)
{
	char temp[2][32];
	int i;

	Sleep(500);
	for (int ct = rtm; ct < HE_MAX; ++ct)
	{
		strcpy_s(temp[0], 32, H_code[ct]);
		sprintf_s(temp[1], 32, "%d", H[ct]);
		DataGridView_AddRow(hWndHelpElementsDataGrid, (LPSTR)temp, 2);
	}

	return 0;
}

LRESULT CALLBACK WindowProcTabHelpElements(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect, irect;
	char temp[3][32];
	int i, col, row;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hWndHelpElementsDataGrid = CreateWindowEx((DWORD)NULL, "DGridVwClass",
			"HelpElements",
			WS_CHILD, rect.left, rect.top, rect.right - rect.left - 6, rect.bottom - 6, hWnd, NULL, hMainInstance, NULL);
		DataGridView_AddColumn(hWndHelpElementsDataGrid, 1, 150, (LPSTR)"Hulp elementen");
		DataGridView_AddColumn(hWndHelpElementsDataGrid, 2, 75, (LPSTR)"Actueel");

		H_old = malloc(sizeof(short) * HE_MAX);

		int me = 0;
		for (me = 0; me < HE_MAX; ++me)
		{
			strcpy_s(temp[0], 32, H_code[me]);
			sprintf_s(temp[1], 32, "%d", H[me]);
			DataGridView_AddRow(hWndHelpElementsDataGrid, (LPSTR)temp, 2);
			ListView_GetItemRect(hWndHelpElementsDataGrid, me, &irect, LVIR_BOUNDS);
			if (irect.bottom >= rect.bottom - 6) break;
		}
		CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			AddRemainingHelpElements,   // thread function name
			me + 1,                 // argument to thread function 
			0,                      // use default creation flags 
			NULL);                  // returns the thread identifier 

		DataGridView_DisplayRowHeaders(hWndHelpElementsDataGrid, TRUE);
		DataGridView_ExtendLastColumn(hWndHelpElementsDataGrid, TRUE, 400, 100);
		DataGridView_SetResizableHeader(hWndHelpElementsDataGrid, 1);
		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);

		if (hWndHelpElementsDataGrid)
		{
			GetClientRect(hWnd, &rect);
			SendMessage(hWndHelpElementsDataGrid, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right - rect.left - 6, rect.bottom - rect.top - 6));
			InvalidateRect(hMainTab, &rect, TRUE);
		}
		return 0;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hWndHelpElementsDataGrid, SW_SHOW);
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
		free(H_old);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TabHelpElementsUpdate()
{
	char ctemp[8];
	RECT rect, topRect;
	if (!hWndHelpElementsDataGrid) return;

	int c1 = ListView_GetColumnWidth(hWndHelpElementsDataGrid, 0);
	int c2 = ListView_GetColumnWidth(hWndHelpElementsDataGrid, 1);

	int top = ListView_GetTopIndex(hWndHelpElementsDataGrid);
	ListView_GetItemRect(hWndHelpElementsDataGrid, top, &topRect, LVIR_BOUNDS);
	RECT winRect;
	GetWindowRect(hWndHelpElementsDataGrid, &winRect);

	for (int ct = top; ct < HE_MAX; ++ct)
	{
		ListView_GetItemRect(hWndHelpElementsDataGrid, ct, &rect, LVIR_BOUNDS);
		if (H[ct] != H_old[ct] && rect.top >= topRect.top && rect.bottom <= winRect.bottom + 20)
		{
			sprintf_s(ctemp, 8, "%d", H[ct]);
			ListView_SetItemText(hWndHelpElementsDataGrid, ct, 1, ctemp);
		}
		else
		{
			H_old[ct] = H[ct];
		}
	}

	for (int ct = 0; ct < HE_MAX; ++ct)
	{
		if (H[ct] != H_old[ct])
		{
			H_old[ct] = H[ct];
		}
	}
}

void TabHelpElementsReset()
{

}
