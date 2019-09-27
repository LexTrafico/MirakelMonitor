#pragma once

#include <Windows.h>
#include <stdio.h>

#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabMemoryElements(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabMemoryElementsUpdate();
void TabMemoryElementsReset();
DWORD WINAPI AddRemainingMemoryElements(int rtm);
