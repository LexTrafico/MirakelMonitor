#pragma once

#include <Windows.h>
#include <stdio.h>

#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabCounters(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabCountersUpdate();
void TabCountersReset();
short CALLBACK tabCT_ischanged(int prm);
DWORD WINAPI AddRemainingCounters(int rtm);
