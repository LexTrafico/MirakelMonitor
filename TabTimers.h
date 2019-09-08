#pragma once

#include <Windows.h>
#include <stdio.h>

#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabTimers(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabTimersUpdate();
void TabTimersReset();
short CALLBACK tabTM_ischanged(int prm);
DWORD WINAPI AddRemainingTimers(int rtm);
