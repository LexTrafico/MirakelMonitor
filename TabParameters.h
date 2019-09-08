#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabParameters(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabParametersUpdate();
void TabParametersReset();
short CALLBACK tabPRM_ischanged(int prm);
DWORD WINAPI AddRemainingParameters(int rtm);
