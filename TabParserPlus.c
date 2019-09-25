#include "Richedit.h"
#include "TabParserPlus.h"

static HWND hParserPlusEdit;
static HWND hParserPlusLog;

static HFONT hFont;
static int iParserPlusTop, iParserPlusLeft, iParserPlusRight, iParserPlusBottom;
static char * lpstLogText = NULL;
static char ** lpstCommands = NULL;
static int iCurrentCommand = 0;
static int iCommandCount = 0;
static int iDisplayedCommand = 0;
static int iTabCount = 0;
void TabParserPlusCreateEditControl(HWND hWnd);

LRESULT CALLBACK WindowProcTabParserPlus(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	int i, col, row;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);

		iParserPlusTop = rect.top;
		iParserPlusLeft = rect.left;
		iParserPlusRight = rect.right;
		iParserPlusBottom = rect.bottom;

		lpstLogText = (char *)malloc(sizeof(char) * 32768);
		lpstLogText[0] = '\0';
		lpstCommands = (char **)malloc(sizeof(char *) * 50);
		for (int i = 0; i < 50; ++i)
		{
			lpstCommands[i] = NULL;
		}

		LoadLibrary("riched20.dll");

		hParserPlusLog = CreateWindowEx(0, TEXT("EDIT"), "",
			WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
			0, iParserPlusBottom - 25, iParserPlusRight, 20,
			hWnd,
			(HMENU)PPLOG, hMainInstance, NULL);

		TabParserPlusCreateEditControl(hWnd);

		SendMessage(hParserPlusLog, EM_SETLIMITTEXT, PARSERLOGMAX, 0);

		hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
		SendMessage(hParserPlusLog, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(hParserPlusEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

		return 0;

	case WM_SIZE:
		GetClientRect(hMainTab, &rect);
		TabCtrl_AdjustRect(hMainTab, 0, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);

		GetClientRect(hWnd, &rect);

		iParserPlusTop = rect.top;
		iParserPlusLeft = rect.left;
		iParserPlusRight = rect.right;
		iParserPlusBottom = rect.bottom;

		SetWindowPos(hParserPlusEdit, NULL, 0, iParserPlusBottom - 20, iParserPlusRight, 20, NULL);
		SetWindowPos(hParserPlusLog, NULL, 0, iParserPlusTop, iParserPlusRight, iParserPlusBottom - 25, NULL);
		return 0;

	case MIRMSG_TABCHANGE:
		switch (wParam)
		{
		case SW_SHOW:
			ShowWindow(hParserPlusEdit, SW_SHOW);
			ShowWindow(hParserPlusLog, SW_SHOW);
			break;
		}
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case PPEDIT:	
			return 1;
		}
		return 1;

	case WM_DESTROY:
		free(lpstLogText);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

extern short uber_getc(void);
extern short iber_puts(const char *str);
WNDPROC oldEditProc;

LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
		{
			char temp[1024];
			SendMessage(hParserPlusEdit, WM_GETTEXT, (WPARAM)1024, (LPARAM)&temp);
			iber_puts(temp);
			if (lpstCommands[iCurrentCommand] != NULL)
			{
				free(lpstCommands[iCurrentCommand]);
				lpstCommands[iCurrentCommand] = NULL;
			}
			lpstCommands[iCurrentCommand] = (char*)malloc(sizeof(char)*(strlen(temp) + 1));
			sprintf_s(lpstCommands[iCurrentCommand], strlen(temp) + 1, "%s", temp);
			iDisplayedCommand = iCurrentCommand;
			++iCurrentCommand;
			if (iCurrentCommand >= 50) iCurrentCommand = 0;
			iber_puts("\n");
			if (iCommandCount < 50) ++iCommandCount;
			SendMessageA(hParserPlusEdit, EM_SETSEL, 0, -1); //Select all
			return 0;
		}
		case VK_UP:
		{
			int t = iDisplayedCommand;
			char temp[1024]; // , temp2[1];
			iDisplayedCommand--;
			if (iDisplayedCommand < 0) iDisplayedCommand = 50 - 1;
			int diff = iCurrentCommand - iDisplayedCommand - 1;
			if (diff < 0) diff += 50;
			if (iDisplayedCommand == iCurrentCommand || lpstCommands[iDisplayedCommand] == NULL ||
				diff > iCommandCount)
			{
				iDisplayedCommand = t;
				return 0;
			}
			sprintf_s(temp, 1024, "%s", lpstCommands[iDisplayedCommand]);
			SendMessage(hParserPlusEdit, WM_SETTEXT, NULL, (LPARAM)&temp);
			SendMessageA(hParserPlusEdit, EM_SETSEL, 0, -1); //Select all
			return 0;
		}
		case VK_DOWN:
		{
			int t = iDisplayedCommand;
			char temp[1024]; // , temp2[1];
			iDisplayedCommand++;
			if (iDisplayedCommand >= 50) iDisplayedCommand = 0;
			int diff = iCurrentCommand - iDisplayedCommand - 1;
			if (diff < 0) diff += 50;
			if (iDisplayedCommand == iCurrentCommand || lpstCommands[iDisplayedCommand] == NULL ||
				diff > iCommandCount)
			{
				iDisplayedCommand = t;
				return 0;
			}
			sprintf_s(temp, 1024, "%s", lpstCommands[iDisplayedCommand]);
			SendMessage(hParserPlusEdit, WM_SETTEXT, NULL, (LPARAM)&temp);
			SendMessageA(hParserPlusEdit, EM_SETSEL, 0, -1); //Select all
			return 0;
		}
		case VK_TAB:
		{
			char temp[1024]; // , temp2[1];
			char type[9]; // , temp2[1];
			char item[128]; // , temp2[1];
			SendMessage(hParserPlusEdit, WM_GETTEXT, (WPARAM)1024, (LPARAM)&temp);
			int i = 0;
			for (i = 0; i < strlen(temp) && i < 8; ++i)
			{
				if (temp[i] == ' ') break;
				type[i] = tolower(temp[i]);
			}
			type[i] = '\0';
			int j = 0;
			for (++i; i < strlen(temp) && j < 127; ++i, ++j)
			{
				if (temp[i] == ' ') break;
				item[j] = tolower(temp[i]);
			}
			item[j] = '\0';
			int max = 0;
			const char ** code = NULL;
			if (strstr(type, "fc") || strstr(type, "mlfc") || 
				strstr(type, "tfg") || strstr(type, "tvg") ||
				strstr(type, "mlafc") || strstr(type, "mlbfc") || strstr(type, "mlcfc"))
			{
				max = FC_MAX;
				code = FC_code;
			}
			else if (strstr(type, "dp") || strstr(type, "tdh"))
			{
				max = DP_MAX;
				code = D_code;
			}
			else if (strstr(type, "prm"))
			{
				max = PRM_MAX;
				code = PRM_code;
			}
			else if (strstr(type, "is"))
			{
				max = IS_MAX;
				code = IS_code;
			}
			else if (strstr(type, "us"))
			{
				max = US_MAX;
				code = US_code;
			}
			else if (strstr(type, "t") || strstr(type, "tm"))
			{
				max = TM_MAX;
				code = T_code;
			}
			else if (strstr(type, "c") || strstr(type, "ct"))
			{
				max = CT_MAX;
				code = C_code;
			}
			else if (strstr(type, "me"))
			{
				max = ME_MAX;
				code = MM_code;
			}
			else if (strstr(type, "he"))
			{
				max = HE_MAX;
				code = H_code;
			}
			if (max == 0) return 0;
			int elem = 0;
			for (; elem < max; ++elem)
			{
				if (strstr(code[elem], item)/* == code[elem][0]*/) break;
			}
			/*if (iTabCount < elem) */iTabCount = elem;
			if (strlen(code[elem]) == strlen(item))
			{
				if (HIWORD(GetKeyState(VK_SHIFT)) & 0x8000) //||
					//HIWORD(GetKeyState(VK_RSHIFT)) == 1 ||
					//HIWORD(GetKeyState(VK_LSHIFT)) == 1)
				{
					--iTabCount;
					if (iTabCount < 0) iTabCount = max - 1;
				}
				else
				{
					++iTabCount;
					if (iTabCount >= max) iTabCount = 0;
				}
			}
			sprintf_s(temp, 1024, "%s %s", type, code[iTabCount]);
			SendMessage(hParserPlusEdit, WM_SETTEXT, NULL, (LPARAM)&temp);
			SendMessageA(hParserPlusEdit, EM_SETSEL, 0, -1); //Select all
			return 0;
		}
		}
	default:
		return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
	}
	return 0;
}


void TabParserPlusCreateEditControl(HWND hWnd)
{
	hParserPlusEdit = CreateWindowEx(0, TEXT("EDIT"), "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		0, 0, 100, 100, hWnd, (HMENU)PPEDIT, GetModuleHandle(NULL), NULL);

	oldEditProc = (WNDPROC)SetWindowLongPtr(hParserPlusEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);
}

void TabParserPlusUpdate()
{
	if (!lpstLogText || !hParserPlusLog) return;
	short c = uber_getc();
	int i = 0, j = 0;
	while (c != -1)
	{
		lpstLogText[i] = c;
		++i; ++j;
		c = uber_getc();
	}
	//lpstLogText[0] = 'a' + i % 26;
	if (j != 0)
	{
		int l = SendMessage(hParserPlusLog, WM_GETTEXTLENGTH, 0, 0); //Length
		// backspace!
		if (lpstLogText[0] == 0x08)
		{
			SendMessage(hParserPlusLog, EM_SETSEL, l > 0 ? l - 1 : 0, -1); //Select last char
			SendMessage(hParserPlusLog, EM_REPLACESEL, 0, (LPARAM)(""));
			SendMessage(hParserPlusLog, EM_SCROLLCARET, 0, 0); //Set scrollcaret to the current Pos
		}
		else
		{
			lpstLogText[i] = '\0';
			if ((l + i) >= PARSERLOGMAX)
			{
				char * temp3 = malloc(sizeof(char) * (PARSERLOGMAX / 10));
				int k = 0;
				unsigned long t = 0;
				SendMessage(hParserPlusLog, WM_GETTEXT, (WPARAM)(PARSERLOGMAX / 10), (LPARAM)temp3);
				while (t < (PARSERLOGMAX / 10) && k < (PARSERLOGMAX / 500))
				{
					++t;
					k += (temp3[t] == '\n' ? 1 : 0);
				}
				SendMessage(hParserPlusLog, EM_SETSEL, 0, t + 1);
				SendMessage(hParserPlusLog, EM_REPLACESEL, 0, (LPARAM)(""));
				free(temp3);
			}
			SendMessage(hParserPlusLog, EM_SETSEL, 0, -1); //Select all
			SendMessage(hParserPlusLog, EM_SETSEL, -1, -1);//Unselect and stay at the end pos
			SendMessage(hParserPlusLog, EM_REPLACESEL, 0, (LPARAM)(lpstLogText));
			SendMessage(hParserPlusLog, EM_SCROLLCARET, 0, 0); //Set scrollcaret to the current Pos
		}
	}
}

void TabParserPlusReset()
{
}