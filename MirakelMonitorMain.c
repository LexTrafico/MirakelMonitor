#include "MirakelMonitorMain.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		OnSize(hMainTab, lParam);
		break;
	case WM_NOTIFY:
		NotifyMainTab(lParam);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateMainTabControl(HWND hwndParent, HINSTANCE hInst)
{
	RECT rcClient;
	HWND hwndTab;
	TCITEM tie;
	int i;
	TCHAR achTemp[256];  // Temporary buffer for strings.

	// Get the dimensions of the parent window's client area, and 
	// create a tab control child window of that size.
	GetClientRect(hwndParent, &rcClient);
	hwndTab = CreateWindow(WC_TABCONTROL, "",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, rcClient.right, rcClient.bottom,
		hwndParent, NULL, hInst, NULL);
	if (hwndTab == NULL)
	{
		return NULL;
	}

	// Add tabs for each day of the week. 
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = achTemp;

	for (i = 0; i < TAB_MAX; i++)
	{
		// Load the day string from the string resources. Note that
		// g_hInst is the global instance handle.
		sprintf_s(achTemp, 256, TAB_MAIN_TITLE[i]);
		if (TabCtrl_InsertItem(hwndTab, i, &tie) == -1)
		{
			DestroyWindow(hwndTab);
			return NULL;
		}
	}
	return hwndTab;
}

HRESULT OnSize(HWND hwndTab, LPARAM lParam)
{
	if (hwndTab == NULL)
		return E_INVALIDARG;

	// Resize the tab control to fit the client are of main window.
	if (!SetWindowPos(hwndTab, HWND_TOP, 0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), SWP_SHOWWINDOW))
		return E_FAIL;

	RECT r;
	GetClientRect(hwndTab, &r);
	TabCtrl_AdjustRect(hwndTab, 0, &r);

	for (int i = 0; i < TAB_MAX; i++)
	{
		SendMessage(hTabs[i], WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(r.right - r.left, r.bottom - r.top));
	}

	return S_OK;
}

BOOL NotifyMainTab(LPARAM lParam)
{
	switch (((LPNMHDR)lParam)->code)
	{
	case TCN_SELCHANGING:
		// Return FALSE to allow the selection to change.
		return FALSE;

	case TCN_SELCHANGE:
	{
		int iPage = TabCtrl_GetCurSel(hMainTab);

		for (int i = 0; i < TAB_MAX; i++)
		{
			if (i == iPage) ShowWindow(hTabs[i], SW_SHOW);
			else ShowWindow(hTabs[i], SW_HIDE);
		}
		break;
	}
	}
	return TRUE;
}

BOOL CALLBACK FindParentWindow(HWND hwnd, LPARAM lParam)
{
    static TCHAR buffer[50];
    GetWindowText(hwnd, buffer, 50);
    if(cParentWinName != NULL && _tcsstr(buffer, cParentWinName)) {
		hParent = hwnd;
        return FALSE;
    }
    return TRUE;
}

ATOM RegisterMirakelMonitorClass(HINSTANCE hInstance)
{
	WNDCLASS wcc;
	wcc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcc.lpfnWndProc = (WNDPROC)WindowProc;
	wcc.cbClsExtra = 0;
	wcc.cbWndExtra = 0;
	wcc.hInstance = hInstance;
	wcc.hIcon = NULL; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MIR));
	wcc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcc.hbrBackground = NULL; //backgroundcol;
	wcc.lpszMenuName = NULL;// MAKEINTRESOURCE(IDR_MIRAKEL_MENU);
	wcc.lpszClassName = "MirakelMonitorClass";

	return RegisterClass(&wcc);
}

int MirakelMonitor_init(char * lpWindowName)
{
	// Register the window class.
	const char CLASS_NAME[] = "MirakelMonitorClass";

	WNDCLASS wc;
	DWORD dwStyle;
	CREATESTRUCT css;
	INITCOMMONCONTROLSEX icex;

	dwStyle = (DWORD)(WS_VISIBLE | WS_SIZEBOX | WS_OVERLAPPEDWINDOW);

	cParentWinName = lpWindowName;
	EnumWindows(FindParentWindow, (LONG)NULL);

	if (hParent == NULL)
	{
		return -1;
	}

	hMainInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

	int e = RegisterMirakelMonitorClass(hMainInstance);

	hMainWin = CreateWindow(
		CLASS_NAME,                     // Window class
		"Mirakel 2.0",                  // Window text
		dwStyle,                        // Window style

		// Size and position
		0, 0, 200, 200,

		hParent,    // Parent window    
		NULL,       // Menu
		hMainInstance,  // Instance handle
		&css        // Additional application data
	);

	if (hMainWin == NULL)
	{
		return GetLastError();
	}

	// Initialize common controls.
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);
	
	Initialize(hMainWin);
	CreateBrushes();
	
	hMainTab = CreateMainTabControl(hMainWin, hMainInstance);
	hTabs[TAB_WACHTTIJDEN] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabWachttijdClass", (WNDPROC)WindowProcTabWachttijden);
	hTabs[TAB_FASENLOG] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabFasenlogClass", (WNDPROC)WindowProcTabFasenlog);

	for (int i = 0; i < TAB_MAX; i++)
	{
		SetParent(hTabs[i], hMainTab);
	}

	int iPage = TabCtrl_GetCurSel(hMainTab);

	for (int i = 0; i < TAB_MAX; i++)
	{
		if (i == iPage) ShowWindow(hTabs[i], SW_SHOW);
		else ShowWindow(hTabs[i], SW_HIDE);
	}

	return 0;
}

void MirakelMonitor()
{
	RECT rect;

	TabWachttijdenUpdate();

	GetClientRect(hMainWin, &rect);
	InvalidateRect(hMainWin, &rect, TRUE);
}