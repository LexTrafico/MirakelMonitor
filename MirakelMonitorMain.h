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
#include <time.h>

//#include "TabFasebewakingsTijden.h"
//#include "TabFasenlog.h"
#include "TabParameters.h"
#include "TabSwitches.h"
#include "TabTimers.h"
#include "TabCounters.h"

#define TAB_TIMERS      0
#define TAB_COUNTERS    1
#define TAB_PARAMETERS  2
#define TAB_SWITCHES    3
//#define TAB_WACHTTIJDEN 2
//#define TAB_FASENLOG    3
#define TAB_MAX         4

HWND hParent = NULL;
HWND hMainWin = NULL;
HWND hMainTab = NULL;
HWND hTabs[TAB_MAX];
HINSTANCE hMainInstance;
char * cParentWinName;
int iCharWidth, iCharHeight;

char bInitialized = FALSE;
int update_monitor = 0;
clock_t last_clock = 0;

char lpszTemp1[SZBUFFERSIZE];

const char * TAB_MAIN_TITLE[TAB_MAX] =
{
	"TM",
	"CT",
	"PRM",
	"SCH",
	//"TFB",
	//"Log",
};

CONTROLLERSTRUCT * Controller;

unsigned long CALLBACK MirGetCTTime(void);
struct fasenlogentry * CALLBACK MirGetCCOLFCState(short fc);
LRESULT CALLBACK WindowProc(HWND hWndMir, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateMainTabControl(HWND hwndParent, HINSTANCE hInst);
ATOM RegisterSomeClass(HINSTANCE hInstance, char * className, WNDPROC proc);
HRESULT OnSize(HWND hwndTab, LPARAM lParam);
