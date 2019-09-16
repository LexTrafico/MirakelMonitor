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
	HFONT hTabFont;

	// Get the dimensions of the parent window's client area, and 
	// create a tab control child window of that size.
	GetClientRect(hwndParent, &rcClient);
	hwndTab = CreateWindow(WC_TABCONTROL, "MirakelMainTab",
		WS_CHILD | WS_VISIBLE | TCS_FLATBUTTONS | TCS_BUTTONS | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, rcClient.right, rcClient.bottom,
		hwndParent, NULL, hInst, NULL);
	if (hwndTab == NULL)
	{
		return NULL;
	}

	// Set tab text font
	hTabFont = CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	SendMessage(hwndTab, WM_SETFONT, (WPARAM)hTabFont, (LPARAM)TRUE);

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
	case TCN_SELCHANGE:
	{
		iSelectedTab = TabCtrl_GetCurSel(hMainTab);
		for (int i = 0; i < TAB_MAX; i++)
		{
			if (i == iSelectedTab)
			{
				ShowWindow(hTabs[i], SW_SHOW);
				SendMessage(hTabs[i], MIRMSG_TABCHANGE, (WPARAM)SW_SHOW, 0);
			}
			else
			{
				ShowWindow(hTabs[i], SW_HIDE);
				SendMessage(hTabs[i], MIRMSG_TABCHANGE, (WPARAM)SW_HIDE, 0);
			}
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
	wcc.hIcon = NULL;
	wcc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcc.hbrBackground = NULL;
	wcc.lpszMenuName = NULL;
	wcc.lpszClassName = "MirakelMonitorClass";

	return RegisterClass(&wcc);
}

int MirakelMonitor_init(char * lpWindowName)
{
	// Register the window class.
	const char CLASS_NAME[] = "MirakelMonitorClass";

	// allocate memory
	Controller = (CONTROLLERSTRUCT *)malloc(sizeof(CONTROLLERSTRUCT));

	Controller->lpWindowName = (char *)malloc((strlen(lpWindowName) + 1) * sizeof(char));
	int ii = (strlen(lpWindowName) + 1);
	strcpy_s((char *)Controller->lpWindowName, strlen(lpWindowName) + 1, lpWindowName);

	WNDCLASS wc;
	char name[128];
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

	sprintf_s(name, 128, "Mirakel monitor 2.0 - %s", lpWindowName);
	hMainWin = CreateWindow(
		CLASS_NAME,                     // Window class
		name,                           // Window text
		dwStyle,                        // Window style

		// Size and position
		0, 0, 400, 500,

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
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_TAB_CLASSES | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	Initialize(hMainWin);
	InitDataGridView(hMainInstance);

	CreateBrushes();
	
	hMainTab = CreateMainTabControl(hMainWin, hMainInstance);
	hTabs[TAB_TIMERS] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabTimersClass", (WNDPROC)WindowProcTabTimers);
	hTabs[TAB_COUNTERS] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabCountersClass", (WNDPROC)WindowProcTabCounters);
	hTabs[TAB_PARAMETERS] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabParametersClass", (WNDPROC)WindowProcTabParameters);
	hTabs[TAB_SWITCHES] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabSwitchesClass", (WNDPROC)WindowProcTabSwitches);
	hTabs[TAB_WACHTTIJDEN] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabWachttijdClass", (WNDPROC)WindowProcTabWachttijden);
	//hTabs[TAB_FASENLOG] = CreateTabDisplayWindow(hMainTab, hMainInstance, "MirakelTabFasenlogClass", (WNDPROC)WindowProcTabFasenlog);

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

	bInitialized = TRUE;

	return 0;
}

void MirakelMonitor()
{
	RECT rect;

	if (!bInitialized) return;

	if (CCOL_Time_Speed_Halt & 0x10 ||
		CCOL_Time_Speed_Halt == 1 ||
		CCOL_Time_Speed_Halt == 2 && update_monitor % 2 == 0 ||
		CCOL_Time_Speed_Halt == 3 && update_monitor % 5 == 0 ||
		CCOL_Time_Speed_Halt == 4 && update_monitor == 0 ||
		((CCOL_Time_Speed_Halt == 5 || CCOL_Time_Speed_Halt == 6) && 
		 (last_clock == 0 || (clock() - last_clock) >= (CLOCKS_PER_SEC / 10))))
	{
		TabTimersUpdate();
		TabCountersUpdate();
		TabParametersUpdate();
		TabSwitchesUpdate();
		if (CCOL_Time_Speed_Halt == 5 || CCOL_Time_Speed_Halt == 6)
		{
			last_clock = clock();
		}

		if (iSelectedTab == TAB_WACHTTIJDEN)
		{
			GetClientRect(hTabs[TAB_WACHTTIJDEN], &rect);
			InvalidateRect(hTabs[TAB_WACHTTIJDEN], &rect, TRUE);
		}
	}
	// update always
	TabWachttijdenUpdate();
	++update_monitor;
	if (update_monitor == 10) update_monitor = 0;
}

// Function to get time from controller
unsigned long CALLBACK MirGetCTTime(void)
{
	unsigned long tm;
	static int te = 0;

	if (TE) te += TE;
	if (TS) te = 0;

	tm = (((CIF_KLOK[CIF_UUR] & 0xff) << 24) +
		((CIF_KLOK[CIF_MINUUT] & 0xff) << 16) +
		((CIF_KLOK[CIF_SECONDE] & 0xff) << 8) +
		(te & 0xff));

	return tm;
}

// Function to retrieve FC state from controller
struct fasenlogentry * CALLBACK MirGetCCOLFCState(short fc)
{
	static struct fasenlogentry fle;
	static int te;

	if (TE) te = ++te % 10;
	if (TS) te = 0;

	fle.state = CG[fc];
	fle.aanvr = A[fc];
	fle.ar = AR[fc];

	return &fle;
}

// Function to _code for CCOL element
const LPCSTR CALLBACK MirGiveCCOLString(short type, short elem)
{
	switch (type)
	{
	case TYPE_FC:	return FC_code[elem];
	case TYPE_DP:   return D_code[elem];
	case TYPE_SCH:	return SCH_code[elem];
	case TYPE_PRM:	return PRM_code[elem];
	case TYPE_TM:	return T_code[elem];
	case TYPE_IS:	return IS_code[elem];
	case TYPE_US:	return US_code[elem];
	case TYPE_ME:	return MM_code[elem];
	case TYPE_HE:	return H_code[elem];
	case TYPE_CT:	return C_code[elem];
	default:		return NULL;
	}
}