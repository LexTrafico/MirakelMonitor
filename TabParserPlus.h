#pragma once

#include <Windows.h>
#include <stdio.h>

#include "MirakelMonitorUtilities.h"

LRESULT CALLBACK WindowProcTabParserPlus(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabParserPlusUpdate();
void TabParserPlusReset();

//#define PARSERLOGMAX 1048576
//#define PARSERLOGMAX 131072
#define PARSERLOGMAX 524288
