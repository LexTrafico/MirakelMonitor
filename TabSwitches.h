#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabSwitches(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabSwitchesUpdate();
void TabSwitchesReset();
short CALLBACK tabSCH_ischanged(int prm);
DWORD WINAPI AddRemainingSwitches(int rtm);
