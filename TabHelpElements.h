#pragma once

#include <Windows.h>
#include <stdio.h>

#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabHelpElements(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabHelpElementsUpdate();
void TabHelpElementsReset();
DWORD WINAPI AddRemainingHelpElements(int rtm);
