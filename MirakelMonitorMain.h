#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include "MirakelTypes.h"
#include "MirakelMonitor.h"
#include "MirakelMonitorUtilities.h"
#include "CCOLDefines.h"
#include <CommCtrl.h>
#include <stdio.h>

#include "TabFasebewakingsTijden.h"
#include "TabFasenlog.h"

#define TAB_WACHTTIJDEN 0
#define TAB_FASENLOG    1
#define TAB_MAX         2

HWND hParent = NULL;
HWND hMainWin = NULL;
HWND hMainTab = NULL;
HWND hTabs[TAB_MAX];
HINSTANCE hMainInstance;
char * cParentWinName;
int iCharWidth, iCharHeight;

char lpszTemp1[SZBUFFERSIZE];

const char * TAB_MAIN_TITLE[TAB_MAX] =
{
	"TFB",
	"Log"
};

CONTROLLERSTRUCT * Controller;

unsigned long CALLBACK MirGetCTTime(void);
struct fasenlogentry * CALLBACK MirGetCCOLFCState(short fc);
LRESULT CALLBACK WindowProc(HWND hWndMir, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateMainTabControl(HWND hwndParent, HINSTANCE hInst);
ATOM RegisterSomeClass(HINSTANCE hInstance, char * className, WNDPROC proc);
HRESULT OnSize(HWND hwndTab, LPARAM lParam);